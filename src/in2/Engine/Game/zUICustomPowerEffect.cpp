#include "xCommon.h"
#include "zUICustomPowerEffect.h"

#include "xRand.h"
#include "xRenderState.h"
#include "zUIImage.h"
#include "zUIMotion.h"

xTWEAKABLE F32 POWER_EFFECT_MIN_WAIT = 3.0f;
xTWEAKABLE F32 POWER_EFFECT_MAX_WAIT = 20.0f;
xTWEAKABLE F32 POWER_EFFECT_MIN_SCROLL = 3.0f;
xTWEAKABLE F32 POWER_EFFECT_MAX_SCROLL = 4.0f;

zUICustomPowerEffect::zUICustomPowerEffect(zUI* ui) : zUICustom(ui)
{
}

void zUICustomPowerEffect::Reset()
{
    waitTime = xrand_RandomFloatRange(POWER_EFFECT_MIN_WAIT, POWER_EFFECT_MAX_WAIT);
    scrollTime = xrand_RandomFloatRange(POWER_EFFECT_MIN_SCROLL, POWER_EFFECT_MAX_SCROLL);
    scrollTotal = scrollTime;
}

void zUICustomPowerEffect::PreUpdate(F32 dt)
{
    if (waitTime >= 0.0f)
        waitTime -= dt;
    if (waitTime <= 0.0f) {
        waitTime = 0.0f;
        scrollTime -= dt;
        if (scrollTime < 0.0f) {
            waitTime = xrand_RandomFloatRange(POWER_EFFECT_MIN_WAIT, POWER_EFFECT_MAX_WAIT);
            scrollTime = xrand_RandomFloatRange(POWER_EFFECT_MIN_SCROLL, POWER_EFFECT_MAX_SCROLL);
            scrollTotal = scrollTime;
        }
    }
}

bool zUICustomPowerEffect::PreRender()
{
    xRenderStateBlendModesSet(rwBLENDONE, rwBLENDZERO);
    return false;
}

void zUICustomPowerEffect::PostRender()
{
}

void zUICustomPowerEffect::PreApplyMotionFrame(zUIMotionFrame* frame)
{
    if (waitTime == 0.0f) {
        const zUIImageAsset* asset = ((zUIImage*)ui)->GetAsset();
        F32 span = asset->u2 - asset->u1;
        if (span < 0.0f)
            span -= 1.0f;
        else
            span += 1.0f;
        frame->offsetU = span * ((scrollTotal - scrollTime) / scrollTotal);
    } else {
        frame->offsetU = 0.0f;
    }
}
