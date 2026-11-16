#ifndef ZNPCCHICKEN_H
#define ZNPCCHICKEN_H

#include "zNPCCommon.h"
#include "zNPCMove.h"

class zNPCChickenIdle : public zNPCCommonBehavior
{
public:
    static void create_anim_table(xAnimTable* table);

    virtual const char* getName() { return "zNPCChickenIdle"; }
    virtual void enterBehavior(const zNPCCommonBehavior* prev_behavior);
    virtual void updateBehavior(F32 dt);
    virtual bool EscapeNoEnterTransitionAllowable() { return false; }

    bool SwitchToRun() { return switchToRun; }
    bool SwitchToFly() { return switchToFly; }
    bool WaitTimeIsUp() { return waitTime <= 0.0f; }

private:
    F32 waitTime;
    F32 animTime;
    bool switchToRun;
    bool switchToFly;
};

class zNPCChickenWalk : public zNPCMove
{
public:
    static void create_anim_table(xAnimTable* table);

    virtual const char* getName() { return "zNPCChickenWalk"; }
    virtual void enterBehavior(const zNPCCommonBehavior* prev_behavior);
    virtual void updateBehavior(F32 dt);
    virtual bool EscapeNoEnterTransitionAllowable() { return false; }

    bool HaveArrived() const { return arrived; }
    bool SwitchToRun() { return switchToRun; }
    bool SwitchToFly() { return switchToFly; }

private:
    xVec3 targetPos;
    F32 timeElapsed;
    bool arrived;
    bool switchToRun;
    bool switchToFly;
};

class zNPCChickenRun : public zNPCMove
{
public:
    static void create_anim_table(xAnimTable* table);

    virtual const char* getName() { return "zNPCChickenRun"; }
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

class zNPCChickenFly : public zNPCMove
{
public:
    static void create_anim_table(xAnimTable* table);

    virtual const char* getName() { return "zNPCChickenFly"; }
    virtual void enterBehavior(const zNPCCommonBehavior* prev_behavior);
    virtual void updateBehavior(F32 dt);
    virtual bool EscapeNoEnterTransitionAllowable() { return false; }

private:
    xVec3 targetPos;
    S32 numFeathers;
    F32 featherTime;
};

class zNPCChicken : public zNPC::zNPCCommon
{
public:
    zNPCChicken();

    static void create_anim_table(xAnimTable* table);

    virtual const char* get_type_name() const { return "zNPCChicken"; }
    virtual S32 GetNPCType() { return 3; }
    virtual bool CanBeTargeted() { return false; }
    virtual F32 GetMeshCircleRadius() { return 0.3f; }
    virtual bool FunkyCheatEffective() { return true; }
    virtual F32 GetFunkyCheatScale() { return 2.0f; }
    virtual void init(zNPC::zNPCInstance* init_inst);
    virtual void BuildStateMachine();
    virtual void SetDefaultAndEventStates();
    virtual bool systemEvent(xBase* from, xBase* to, U32 to_event, const F32* to_param, xBase* to_param_widget, U32 to_param_widget_id);

private:
    zNPCChickenIdle* idleState;
    zNPCChickenWalk* walkState;
    zNPCChickenRun* runState;
    zNPCChickenFly* flyState;
};

#endif
