#include "xCommon.h"
#include "zNPCBossUnderminerUM.h"

#include "xAnim.h"
#include "xSpacePartitionTree.h"
#include "xString.h"
#include "zEvent.h"
#include "zHitParameters.h"
#include "zNPC.h"
#include "zNPCBossUnderminerDrill.h"
#include "zNPCCombat.h"
#include "zNPCMgr.h"
#include "zNPCNavigation.h"

static xHierarchyBoundInitData sNPCUnderminerUMCharacterBounds[2] = {
    { 0, 1.3f, zHT_FRONT },
    { -2, 0.0f, 0 },
};

static zNPCCombatInitData sUnderminerUMCombatInitDataBasic = { sNPCUnderminerUMCharacterBounds, 1.3f };

void zNPCBossUnderminerUMIdle::create_anim_table(xAnimTable* table)
{
    xAnimTableNewState(table, "UM_IDLE", AnimState_Loop, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
    xAnimTableNewState(table, "UM_IDLE_ALERT", AnimState_Loop, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
    xAnimTableNewState(table, "UM_IDLE_NUKE", AnimState_Loop, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
    xAnimTableNewState(table, "UM_TAUNT_LOOP_01", AnimState_Loop, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
    xAnimTableNewState(table, "UM_TAUNT_LOOP_02", AnimState_Loop, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
    xAnimTableNewState(table, "UM_TAUNT_LOOP_03", AnimState_Loop, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
    xAnimTableNewState(table, "UM_JUMP", 0, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
    xAnimTableNewState(table, "UM_RAM_READY", 0, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
    xAnimTableNewState(table, "UM_RAM_LOOP", AnimState_Loop, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
    xAnimTableNewState(table, "UM_RAM_STOP", 0, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
    xAnimTableNewState(table, "UM_MONO_HIT_LIGHT", 0, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
    xAnimTableNewState(table, "UM_MONO_HIT_HARD", 0, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
    xAnimTableNewState(table, "UM_COMMAND_TROOPS", 0, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
    xAnimTableNewState(table, "UM_SLASH_HIT", 0, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
    xAnimTableNewState(table, "UM_SLASH_LOOP", AnimState_Loop, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
    xAnimTableNewState(table, "UM_SLASH_READY", 0, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
    xAnimTableNewState(table, "UM_SLASH_STOP", 0, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
    xAnimTableNewState(table, "UM_FREEZE_STOP", 0, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
    xAnimTableNewState(table, "UM_MISSILE_IDLE", AnimState_Loop, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
    xAnimTableNewState(table, "UM_MISSILE_READY", 0, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
    xAnimTableNewState(table, "UM_MISSILE_SHOOT_L", 0, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
    xAnimTableNewState(table, "UM_MISSILE_SHOOT_R", 0, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
    xAnimTableNewState(table, "UM_MISSILE_STOP", 0, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
    xAnimTableNewState(table, "UM_SWING_DIZZY", AnimState_Loop, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
    xAnimTableNewState(table, "UM_SWING_HIT", 0, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
    xAnimTableNewState(table, "UM_SWING_LOOP", AnimState_Loop, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
    xAnimTableNewState(table, "UM_SWING_READY", 0, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
    xAnimTableNewState(table, "UM_SWING_STOP", 0, 0, 1.0f, NULL, NULL, 0.0f, NULL, NULL, NULL, NULL, NULL);
}

void zNPCBossUnderminerUMIdle::enterBehavior(const zNPCCommonBehavior*)
{
    setAnimState("UM_IDLE_ALERT", 0.3f, NULL);
    owner->navigation_behavior->dont_apply_dpos();
    headShown = false;
}

void zNPCBossUnderminerUMIdle::updateBehavior(F32)
{
    if (((zNPCBossUnderminerUM*)owner)->cart->currentStage != eUMBossStageD) {
        syncPosAndAnim();
    }

    owner->navigation_behavior->dont_apply_dpos();

    if (boss->currentStage == eUMBossStageD) {
        setAnimState("UM_HEAD_IDLE", 0.3f, NULL);
        xEntShow(owner);
        headShown = true;
    }
}

void zNPCBossUnderminerUMIdle::syncPosAndAnim()
{
    zNPCBossUnderminerUM* npcUM = (zNPCBossUnderminerUM*)owner;

    if (npcUM->cart == NULL || npcUM->noCartSync) {
        return;
    }

    npcUM->frame->mat = npcUM->cart->frame->mat;

    if (npcUM->model->Anim->Single->State->ID != npcUM->cart->model->Anim->Single->State->ID) {
        setAnimState(npcUM->cart->model->Anim->Single->State->ID, 0.3f, NULL);
    }

    npcUM->model->Anim->Single->Time = npcUM->cart->model->Anim->Single->Time;
}

zNPCBossUnderminerUM::zNPCBossUnderminerUM()
{
}

void zNPCBossUnderminerUM::init(zNPC::zNPCInstance* init_inst)
{
    zNPCCommon::init(init_inst);
    zNPCMgr* mgr = zNPCMgrSelf();
    const char* name = "UM_CART";
    cart = (zNPCBossUnderminerDrill*)mgr->FindNPC(xStrHash(name))->get_data();
    manager.createTransientStack(4);
    BuildStateMachine();
    SetDefaultAndEventStates();
    SetCombat();
    canJump = false;
    canDodge = false;
    canTakeDamage = false;
    damaged = false;
    hitByBolt = false;
    headTimeUp = false;
    noCartSync = false;
}

void zNPCBossUnderminerUM::damage(const zCombatDamageInfo& damageInfo)
{
    if (!ShouldBeInvincible()) {
        zNPCCommon::damage(damageInfo);
        damaged = true;
    }
}

bool zNPCBossUnderminerUM::ShouldBeInvincible()
{
    return true;
}

bool zNPCBossUnderminerUM::CanHelperAITarget()
{
    return false;
}

void zNPCBossUnderminerUM::SetCombat()
{
    zNPCCombatInitData* combatData = GetCombatData();
    if (combatData) {
        F32 scale = 1.0f;
        GetParameter("Scale", &scale, 1.0f);
        char prefix[128] = "";
        zNPCCombatSetCombat(this, id, prefix, combatData, asset->modelInfoID, 1, scale);
    }
}

zNPCCombatInitData* zNPCBossUnderminerUM::GetCombatData()
{
    return &sUnderminerUMCombatInitDataBasic;
}

void zNPCBossUnderminerUM::create_anim_table(xAnimTable* table)
{
    zNPCBossUnderminerUMIdle::create_anim_table(table);
    zNPCCommon::create_anim_table(table);
}

bool zNPCBossUnderminerUM::systemEvent(xBase* from, xBase* to, U32 to_event, const F32* to_param, xBase* to_param_widget, U32 to_param_widget_id)
{
    bool result = true;

    switch (to_event) {
    case eEventBossStageBeganD:
        break;
    default:
        result = zNPCCommon::systemEvent(from, to, to_event, to_param, to_param_widget, to_param_widget_id);
        break;
    }

    return result;
}

void zNPCBossUnderminerUM::BuildStateMachine()
{
    addBehavior(&idleState, -1);
}

void zNPCBossUnderminerUM::SetDefaultAndEventStates()
{
    manager.setDefaultBehavior(&idleState);
}

void zNPCBossUnderminerUM::update_npc(F32 dt)
{
    zNPCCommon::update_npc(dt);

    if (cart->currentStage == eUMBossStageD) {
        headTimer -= dt;
        if (headTimer <= 0.0f) {
            headTimeUp = true;
        }
    }
}
