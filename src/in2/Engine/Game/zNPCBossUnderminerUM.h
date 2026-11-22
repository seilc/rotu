#ifndef ZNPCBOSSUNDERMINERUM_H
#define ZNPCBOSSUNDERMINERUM_H

#include "zNPCCommon.h"
#include "zNPCMove.h"
#include "zNPCTypes.h"

class zNPCBossUnderminerDrill;

class zNPCBossUnderminerUMIdle : public zNPCMove
{
public:
    static void create_anim_table(xAnimTable* table);

    virtual const char* getName() { return "zNPCBossUnderminerUMIdle"; }
    virtual void enterBehavior(const zNPCCommonBehavior* prev_behavior);
    virtual void updateBehavior(F32 dt);
    virtual bool EscapeNoEnterTransitionAllowable() { return false; }
    virtual bool CanBeFrozen() { return false; }
    virtual bool CanBeTargeted() { return false; }

    void syncPosAndAnim();

private:
    bool headShown;
};

class zNPCBossUnderminerUM : public zNPC::zNPCCommon
{
public:
    zNPCBossUnderminerUM();

    static void create_anim_table(xAnimTable* table);

    virtual const char* get_type_name() const { return "zNPCBossUnderminerUM"; }
    virtual S32 GetNPCType() { return eNPCType_BossUnderminerUM; }
    virtual bool CanHelperAITarget();
    virtual zNPCCombatInitData* GetCombatData();
    virtual void init(zNPC::zNPCInstance* init_inst);
    virtual void BuildStateMachine();
    virtual void SetDefaultAndEventStates();
    virtual void update_npc(F32 dt);
    virtual void damage(const zCombatDamageInfo& damageInfo);
    virtual bool systemEvent(xBase* from, xBase* to, U32 to_event, const F32* to_param, xBase* to_param_widget, U32 to_param_widget_id);
    virtual void SetCombat();

    bool ShouldBeInvincible();

    zNPCBossUnderminerUMIdle idleState;
    zNPCBossUnderminerDrill* cart;
    U8 unk4A8[3];
    bool damaged;
    bool hitByBolt;
    U8 unk4AD[7];
    F32 headTimer;
    bool headTimeUp;
    bool noCartSync;
};

#endif
