#ifndef ZUICUSTOM_H
#define ZUICUSTOM_H

#include "types.h"

class zUI;
struct xBase;
struct zUIMotionFrame;

class zUICustom
{
public:
    zUICustom(zUI* ui) : ui(ui) {}

    virtual void Init();
    virtual void Setup();
    virtual void Reset();
    virtual void ResetMotion();
    virtual bool HandleEvent(xBase* from, U32 toEvent, const F32* toParam, xBase* toParamWidget, U32 toParamWidgetID);
    virtual void PreUpdate(F32);
    virtual void PostUpdate(F32);
    virtual bool PreRender();
    virtual void PostRender();
    virtual void Exit();
    virtual bool PreAutoMenu();
    virtual void PreInitMotion();
    virtual void PostInitMotion();
    virtual void PreApplyMotionFrame(zUIMotionFrame*);
    virtual void PostApplyMotionFrame(zUIMotionFrame*);
    virtual bool IsSystem(zUI*) { return false; }
    virtual bool IsSignalAllowed(U32) { return true; }
    virtual bool IsForcePreUpdate() { return false; }

    zUI* GetUI() const
    {
        return ui;
    }

protected:
    zUI* ui;
};

void zSendUIEvent(const char* widgetName, U32 event);

#endif
