#include "xCommon.h"
#include "zNPCRat.h"

#include "xAnim.h"
#include "xGlobals.h"
#include "xMath.h"
#include "xRand.h"
#include "xSpacePartitionTree.h"
#include "zEvent.h"
#include "zHashes.h"
#include "zNPCNavigation.h"
#include "zPlayer.h"

namespace {

U32 gIdleStateNameHashes[] = { HASH_RAT_L_IDLE01, HASH_RAT_L_IDLE02, HASH_RAT_T_STANDUP01 };

}

static bool zNPCRatIdleToRunTest(xEnt* ent, behavior* current, behavior*)
{
    if (((zNPCRatIdle*)current)->WaitTimeIsUp()) {
        return true;
    }

    S32 numPlayers = CURPLAYERS.size();
    for (S32 i = 0; i < numPlayers; i++) {
        xVec3 toPlayer;
        toPlayer.Sub(CURPLAYERS[i]->frame->mat.pos, ent->frame->mat.pos);
        if (toPlayer.length2() <= 25.0f) {
            return true;
        }
    }

    return false;
}

void zNPCRatIdle::create_anim_table(xAnimTable* table)
{
    xAnimTableNewState(table, "RAT_L_IDLE01", AnimState_Loop, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
    xAnimTableNewState(table, "RAT_L_IDLE02", AnimState_Loop, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
    xAnimTableNewState(table, "RAT_T_STANDUP01", AnimState_Loop, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
}

void zNPCRatIdle::enterBehavior(const zNPCCommonBehavior*)
{
    setAnimState("RAT_L_IDLE01", 0.3f, NULL);
    waitTime = xrand_RandomFloatRange(8.0f, 15.0f);
    animTime = xrand_RandomFloatRange(1.0f, 3.0f);
}

void zNPCRatIdle::updateBehavior(F32 dt)
{
    waitTime -= dt;
    animTime -= dt;
    if (animTime <= 0.0f) {
        S32 anim = xrand_RandomChance(0.3f) ? 2 : (xrand_RandomChance(0.5f) ? 0 : 1);
        setAnimState(gIdleStateNameHashes[anim], 0.3f, NULL);
        animTime = xrand_RandomFloatRange(1.0f, 3.0f);
    }
}

static bool zNPCRatRunToIdleTest(xEnt*, behavior* current, behavior*)
{
    return ((zNPCRatRun*)current)->HaveArrived();
}

void zNPCRatRun::create_anim_table(xAnimTable* table)
{
    xAnimTableNewState(table, "RAT_L_RUN01", AnimState_Loop, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
}

void zNPCRatRun::enterBehavior(const zNPCCommonBehavior*)
{
    setAnimState("RAT_L_RUN01", 0.3f, NULL);
    PickDest();
    timeElapsed = 0.0f;
    arrived = false;
}

void zNPCRatRun::updateBehavior(F32 dt)
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

    if (is_stuck() || timeElapsed >= 10.0f) {
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

void zNPCRatRun::PickDest()
{
    F32 d2 = 0.0f;
    xEnt* p = zNPC::FindClosestPlayer(owner, &d2);
    if (p && d2 <= 25.0f) {
        xVec3 toPlayer;
        toPlayer.Sub(*xEntGetPos(p), owner->frame->mat.pos);
        toPlayer.negate();
        toPlayer.NormalizeSafe();
        targetPos.AddScale(owner->frame->mat.pos, toPlayer, 10.0f);
        owner->navigation_behavior->GetClosestPosition(targetPos, targetPos);
    } else {
        PickRandomPos(owner->frame->mat.pos, 10.0f, targetPos);
    }
}

zNPCRat::zNPCRat()
{
}

void zNPCRat::BuildStateMachine()
{
    if (!idleState) {
        idleState = new (eMemStaticType_0, eMemMgrTag_Enemy) zNPCRatIdle;
        addBehavior(idleState, -1);
    }

    if (!moveState) {
        moveState = new (eMemStaticType_0, eMemMgrTag_Enemy) zNPCRatRun;
        addBehavior(moveState, -1);
    }

    idleState->addTest(moveState, zNPCRatIdleToRunTest, "zNPCRatIdleToRunTest", 0.0f);
    moveState->addTest(idleState, zNPCRatRunToIdleTest, "zNPCRatRunToIdleTest", 0.0f);
}

void zNPCRat::init(zNPC::zNPCInstance* init_inst)
{
    zNPCCommon::init(init_inst);
    commonFlags.cast_shadow = false;
    manager.createTransientStack(1);
    BuildStateMachine();
    SetDefaultAndEventStates();
    canJump = false;
    canDodge = false;
    canTakeDamage = false;
}

void zNPCRat::SetDefaultAndEventStates()
{
    manager.setDefaultBehavior(idleState);
}

void zNPCRat::create_anim_table(xAnimTable* table)
{
    zNPCRatIdle::create_anim_table(table);
    zNPCRatRun::create_anim_table(table);
    zNPCCommon::create_anim_table(table);
}

bool zNPCRat::systemEvent(xBase* from, xBase* to, U32 to_event, const F32* to_param, xBase* to_param_widget, U32 to_param_widget_id)
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
