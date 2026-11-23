#include "xCommon.h"
#include "zNPCChicken.h"

#include "xAnim.h"
#include "xGlobals.h"
#include "xMath.h"
#include "xRand.h"
#include "xSpacePartitionTree.h"
#include "zEvent.h"
#include "zHashes.h"
#include "zNPCChickenFX.h"
#include "zPlayer.h"

static void zNPCChickenCheckPlayerApproach(const xEnt* ent, bool& fly, bool& run, F32 dt)
{
    fly = run = false;

    S32 numPlayers = CURPLAYERS.size();
    F32 invDt2 = 1.0f / (dt * dt);
    for (S32 i = 0; i < numPlayers; i++) {
        xVec3 toPlayer;
        toPlayer.Sub(CURPLAYERS[i]->frame->mat.pos, ent->frame->mat.pos);
        if (toPlayer.length2() <= 25.0f) {
            xVec3 playerVel;
            playerVel.Sub(CURPLAYERS[i]->frame->mat.pos, CURPLAYERS[i]->frame->oldmat.pos);
            if (invDt2 * playerVel.length2() <= 15.0f) {
                run = true;
            } else {
                fly = true;
            }
        }
    }
}

static bool zNPCChickenIdleToRunTest(xEnt*, behavior* current, behavior*)
{
    return ((zNPCChickenIdle*)current)->SwitchToRun();
}

static bool zNPCChickenIdleToFlyTest(xEnt*, behavior* current, behavior*)
{
    return ((zNPCChickenIdle*)current)->SwitchToFly();
}

static bool zNPCChickenIdleToWalkTest(xEnt*, behavior* current, behavior*)
{
    if (((zNPCChickenIdle*)current)->inAnimState(HASH_CHICKEN_L_IDLE02, 0.0f, NULL)) {
        return false;
    }

    if (((zNPCChickenIdle*)current)->SwitchToRun() || ((zNPCChickenIdle*)current)->SwitchToFly()) {
        return false;
    }

    if (((zNPCChickenIdle*)current)->WaitTimeIsUp()) {
        return true;
    }

    return false;
}

void zNPCChickenIdle::create_anim_table(xAnimTable* table)
{
    xAnimTableNewState(table, "CHICKEN_L_IDLE01", AnimState_Loop, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
    xAnimTableNewState(table, "CHICKEN_L_IDLE02", AnimState_Loop, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
}

void zNPCChickenIdle::enterBehavior(const zNPCCommonBehavior*)
{
    setAnimState("CHICKEN_L_IDLE01", 0.3f, NULL);
    waitTime = xrand_RandomFloatRange(10.0f, 15.0f);
    animTime = xrand_RandomFloatRange(2.0f, 3.5f);
    switchToRun = switchToFly = false;
}

void zNPCChickenIdle::updateBehavior(F32 dt)
{
    waitTime -= dt;
    animTime -= dt;

    bool fly, run;
    zNPCChickenCheckPlayerApproach(owner, fly, run, dt);
    if (fly) {
        switchToFly = true;
    } else if (run) {
        switchToRun = true;
    } else if (animTime <= 0.0f) {
        if (inAnimState(HASH_CHICKEN_L_IDLE02, 0.0f, NULL)) {
            if (xrand_RandomChance(0.5f)) {
                setAnimState("CHICKEN_L_IDLE01", 0.3f, NULL);
            }
        } else if (xrand_RandomChance(0.5f)) {
            setAnimState("CHICKEN_L_IDLE02", 0.3f, NULL);
        }
        animTime = xrand_RandomFloatRange(2.0f, 3.5f);
    }
}

static bool zNPCChickenWalkToIdleTest(xEnt*, behavior* current, behavior*)
{
    return ((zNPCChickenWalk*)current)->HaveArrived();
}

static bool zNPCChickenWalkToRunTest(xEnt*, behavior* current, behavior*)
{
    return ((zNPCChickenWalk*)current)->SwitchToRun();
}

static bool zNPCChickenWalkToFlyTest(xEnt*, behavior* current, behavior*)
{
    return ((zNPCChickenWalk*)current)->SwitchToFly();
}

void zNPCChickenWalk::create_anim_table(xAnimTable* table)
{
    xAnimTableNewState(table, "CHICKEN_L_WALK01", AnimState_Loop, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
}

void zNPCChickenWalk::enterBehavior(const zNPCCommonBehavior*)
{
    setAnimState("CHICKEN_L_WALK01", 0.3f, NULL);
    PickRandomPos(owner->frame->mat.pos, 2.0f, 3.5f, targetPos);
    arrived = switchToRun = switchToFly = false;
}

void zNPCChickenWalk::updateBehavior(F32 dt)
{
    bool fly, run;
    zNPCChickenCheckPlayerApproach(owner, fly, run, dt);
    if (fly) {
        switchToFly = true;
    } else if (run) {
        switchToRun = true;
    }

    // BUG: timeElapsed is never reset
    timeElapsed += dt;

    if (!switchToFly && !switchToRun) {
        xVec3 toDest;
        toDest.Sub(targetPos, owner->frame->mat.pos);
        if (toDest.length2() <= 0.4f * 0.4f || timeElapsed >= 8.0f) {
            arrived = true;
        } else {
            apply_angle_diff(get_angle_difference(&owner->frame->mat.at, &toDest), dt, 2.0f * PI);
        }
    }
}

static bool zNPCChickenRunToIdleTest(xEnt*, behavior* current, behavior*)
{
    return ((zNPCChickenRun*)current)->HaveArrived();
}

void zNPCChickenRun::create_anim_table(xAnimTable* table)
{
    xAnimTableNewState(table, "CHICKEN_L_RUN01", AnimState_Loop, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
}

void zNPCChickenRun::enterBehavior(const zNPCCommonBehavior*)
{
    setAnimState("CHICKEN_L_RUN01", 0.3f, NULL);
    PickDest();
    arrived = false;
}

void zNPCChickenRun::updateBehavior(F32 dt)
{
    bool getNewDest = false;

    timeElapsed += dt;

    xVec3 toDest;
    toDest.Sub(targetPos, owner->frame->mat.pos);
    if (toDest.length2() <= 0.4f * 0.4f) {
        if (zNPC::CheckForPlayerInRange2(owner, 25.0f)) {
            getNewDest = true;
        } else {
            arrived = true;
        }
    }

    if (is_stuck() || timeElapsed >= 8.0f) {
        getNewDest = true;
    }

    if (!arrived) {
        if (getNewDest) {
            PickDest();
            timeElapsed = 0.0f;
            toDest.Sub(targetPos, owner->frame->mat.pos);
        }
        apply_angle_diff(get_angle_difference(&owner->frame->mat.at, &toDest), dt, 2.0f * PI);
    }
}

void zNPCChickenRun::PickDest()
{
    PickRandomPos(owner->frame->mat.pos, xrand_RandomFloatRange(5.0f, 10.0f), targetPos);
}

static bool zNPCChickenFlyToRunTest(xEnt*, behavior* current, behavior*)
{
    return !((zNPCChickenFly*)current)->hasAnimEnded(NULL);
}

void zNPCChickenFly::create_anim_table(xAnimTable* table)
{
    xAnimTableNewState(table, "CHICKEN_T_FLYFORWARD01", AnimState_Once, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
}

void zNPCChickenFly::enterBehavior(const zNPCCommonBehavior*)
{
    setAnimState("CHICKEN_T_FLYFORWARD01", 0.3f, NULL);

    xVec3 pos = owner->frame->mat.pos;
    pos.y += 1.0f;
    zNPCChickenFXFeather::Emit(pos, xrand_RandomRange(3, 5), 1.0f, &zNPCChickenFXFeather::defaultConfig);

    numFeathers = xrand_RandomRange(2, 4);
    featherTime = 0.0f;
}

void zNPCChickenFly::updateBehavior(F32 dt)
{
    if (numFeathers > 0) {
        featherTime += 10.0f * dt;
        if (featherTime >= 1.0f) {
            S32 emit = (S32)featherTime;
            if (emit > numFeathers) {
                emit = numFeathers;
            }

            xVec3 pos = owner->frame->mat.pos;
            pos.y += 1.0f;
            zNPCChickenFXFeather::Emit(pos, emit, 1.0f, &zNPCChickenFXFeather::defaultConfig);

            featherTime -= emit;
            numFeathers -= emit;
        }
    }
}

zNPCChicken::zNPCChicken()
{
}

void zNPCChicken::init(zNPC::zNPCInstance* init_inst)
{
    zNPCCommon::init(init_inst);
    commonFlags.cast_shadow = false;
    manager.createTransientStack(1);
    BuildStateMachine();
    SetDefaultAndEventStates();
    zNPCChickenFXFeather::Setup();
    canJump = false;
    canDodge = false;
    canTakeDamage = false;
}

void zNPCChicken::BuildStateMachine()
{
    if (!idleState) {
        idleState = new (eMemStaticType_0, eMemMgrTag_Enemy) zNPCChickenIdle;
        addBehavior(idleState, -1);
    }

    if (!runState) {
        runState = new (eMemStaticType_0, eMemMgrTag_Enemy) zNPCChickenRun;
        addBehavior(runState, -1);
    }

    if (!walkState) {
        walkState = new (eMemStaticType_0, eMemMgrTag_Enemy) zNPCChickenWalk;
        addBehavior(walkState, -1);
    }

    if (!flyState) {
        flyState = new (eMemStaticType_0, eMemMgrTag_Enemy) zNPCChickenFly;
        addBehavior(flyState, -1);
    }

    idleState->addTest(runState, zNPCChickenIdleToRunTest, "zNPCChickenIdleToRunTest", 0.0f);
    idleState->addTest(walkState, zNPCChickenIdleToWalkTest, "zNPCChickenIdleToWalkTest", 0.0f);
    idleState->addTest(flyState, zNPCChickenIdleToFlyTest, "zNPCChickenIdleToFlyTest", 0.0f);
    walkState->addTest(runState, zNPCChickenWalkToRunTest, "zNPCChickenWalkToRunTest", 0.0f);
    walkState->addTest(idleState, zNPCChickenWalkToIdleTest, "zNPCChickenWalkToIdleTest", 0.0f);
    walkState->addTest(flyState, zNPCChickenWalkToFlyTest, "zNPCChickenWalkToFlyTest", 0.0f);
    runState->addTest(idleState, zNPCChickenRunToIdleTest, "zNPCChickenRunToIdleTest", 0.0f);
    flyState->addTest(runState, zNPCChickenFlyToRunTest, "zNPCChickenFlyToRunTest", 0.0f);
}

void zNPCChicken::SetDefaultAndEventStates()
{
    manager.setDefaultBehavior(idleState);
}

void zNPCChicken::create_anim_table(xAnimTable* table)
{
    zNPCChickenIdle::create_anim_table(table);
    zNPCChickenRun::create_anim_table(table);
    zNPCChickenWalk::create_anim_table(table);
    zNPCChickenFly::create_anim_table(table);
    zNPCCommon::create_anim_table(table);
}

bool zNPCChicken::systemEvent(xBase* from, xBase* to, U32 to_event, const F32* to_param, xBase* to_param_widget, U32 to_param_widget_id)
{
    switch (to_event) {
    case eEventNPCEnterIdleSpawn:
        inst->StoreSpawnInfo(from, to, to_event, to_param, to_param_widget, to_param_widget_id);
        activate();
        manager.setBehavior(idleState);
        ExitingSpawnState();
        return true;
    }

    return zNPCCommon::systemEvent(from, to, to_event, to_param, to_param_widget, to_param_widget_id);
}
