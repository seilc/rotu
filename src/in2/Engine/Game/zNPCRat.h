#ifndef ZNPCRAT_H
#define ZNPCRAT_H

#include "zNPCCommon.h"
#include "zNPCMove.h"
#include "zNPCTypes.h"

class zNPCRatIdle : public zNPCCommonBehavior
{
public:
    static void create_anim_table(xAnimTable* table);

    virtual const char* getName() { return "zNPCRatIdle"; }
    virtual void enterBehavior(const zNPCCommonBehavior* prev_behavior);
    virtual void updateBehavior(F32 dt);
    virtual bool EscapeNoEnterTransitionAllowable() { return false; }

    bool WaitTimeIsUp() { return waitTime <= 0.0f; }

private:
    F32 waitTime;
    F32 animTime;
};

class zNPCRatRun : public zNPCMove
{
public:
    static void create_anim_table(xAnimTable* table);

    virtual const char* getName() { return "zNPCRatRun"; }
    virtual void enterBehavior(const zNPCCommonBehavior* prev_behavior);
    virtual void updateBehavior(F32 dt);
    virtual bool EscapeNoEnterTransitionAllowable() { return false; }

    bool HaveArrived() const { return arrived; }
    void PickDest();

private:
    xVec3 targetPos;
    F32 timeElapsed;
    bool arrived;
};

class zNPCRat : public zNPC::zNPCCommon
{
public:
    zNPCRat();

    static void create_anim_table(xAnimTable* table);

    virtual const char* get_type_name() const { return "zNPCRat"; }
    virtual S32 GetNPCType() { return eNPCType_Rat; }
    virtual bool CanBeTargeted() { return false; }
    virtual F32 GetMeshCircleRadius() { return 0.1f; }
    virtual bool FunkyCheatEffective() { return true; }
    virtual F32 GetFunkyCheatScale() { return 4.0f; }
    virtual void init(zNPC::zNPCInstance* init_inst);
    virtual void BuildStateMachine();
    virtual void SetDefaultAndEventStates();
    virtual bool systemEvent(xBase* from, xBase* to, U32 to_event, const F32* to_param, xBase* to_param_widget, U32 to_param_widget_id);

private:
    zNPCRatIdle* idleState;
    zNPCRatRun* moveState;
};

#endif
