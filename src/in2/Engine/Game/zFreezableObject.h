#ifndef ZFREEZABLEOBJECT_H
#define ZFREEZABLEOBJECT_H

#include "xEnt.h"
#include "xSndMgr.h"
#include "zCommonPlayer.h"
#include "zFreezableObjectAsset.h"

struct zShrapnelAsset;

namespace xRumble {
struct FreezableObjectEffect;
}

class zFreezableObject : public xEnt
{
public:
    void Freezing();
    static void SetFreezeLevel(zCommonPlayerUpgradeLevel level);
    static void InitDynamic(xBase& data, xDynAsset& asset, size_t asset_size);
    void Init(zFreezableObjectAsset* asset);
    static void SceneEnter();
    void Setup();
    void Reset();
    void On();
    void Off();
    static void Update(xEnt* ent, xScene* sc, F32 dt);
    void Update(F32 dt);
    void EmitFreezeSparks(F32 dt);
    void EmitSparksNow(S32 count, F32 dt);
    void Explode();
    void EmitIceShrapnel(bool alwaysPlaySound);
    static void Render(xEnt* ent);
    void Render();
    static void EventCB(xBase* from, xBase* to, U32 toEvent, const F32* toParam, xBase* toParamWidget,
                        U32 toParamWidgetID);

    static F32 mTimeToFreezeMultiplier;

protected:
    bool mActive;
    bool mCanFreeze;
    zFreezableObjectAsset* mAsset;
    zShrapnelAsset* mShrapnel;
    xBase* mKilledTarget;
    xModelInstance* mIceModel;
    F32 mRotation;
    bool mFreezing;
    bool mThawing;
    F32 mTimeToFreeze;
    F32 mShatterTimer;
    F32 mIceScaleMin;
    F32 mHealth;
    F32 mSparkTimer;
    F32 mSparkDelay;
    bool mSparkRaised;
    xSndHandle mRollSound;
    xRumble::FreezableObjectEffect* mRumble;
};

#endif
