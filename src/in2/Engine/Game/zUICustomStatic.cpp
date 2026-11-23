#include "xCommon.h"
#include "zUICustomStatic.h"

#include "xDebugTweak.h"
#include "xRand.h"
#include "zUI.h"
#include "zUIMotion.h"

xTWEAKABLE F32 STATIC_ALPHA = 128.0f;
xTWEAKABLE F32 STATIC_MIN_OFF_TIME = 3.0f;
xTWEAKABLE F32 STATIC_MAX_OFF_TIME = 7.0f;
xTWEAKABLE F32 STATIC_FADE_IN_TIME = 0.4f;
xTWEAKABLE F32 STATIC_MIN_ON_TIME = 0.2f;
xTWEAKABLE F32 STATIC_MAX_ON_TIME = 0.5f;
xTWEAKABLE F32 STATIC_FADE_OUT_TIME = 0.5f;
xTWEAKABLE F32 STATIC_UV_STRENGTH = 0.05f;

zUICustomStatic::zUICustomStatic(zUI* ui) : zUICustom(ui)
{
}

void zUICustomStatic::Reset()
{
    stage = Stage_Off;
    timer = xrand_RandomFloatRange(STATIC_MIN_OFF_TIME, STATIC_MAX_OFF_TIME);
}

void zUICustomStatic::PreUpdate(F32 dt)
{
    timer -= dt;
    if (timer <= 0.0f)
        switch (stage) {
        case Stage_Off:
            stage = Stage_FadeIn;
            timer = STATIC_FADE_IN_TIME;
            break;

        case Stage_FadeIn:
            stage = Stage_On;
            timer = xrand_RandomFloatRange(STATIC_MIN_ON_TIME, STATIC_MAX_ON_TIME);
            break;

        case Stage_On:
            stage = Stage_FadeOut;
            timer = STATIC_FADE_OUT_TIME;
            break;

        case Stage_FadeOut:
            stage = Stage_Off;
            timer = xrand_RandomFloatRange(STATIC_MIN_OFF_TIME, STATIC_MAX_OFF_TIME);
            break;
        }
}

void zUICustomStatic::PreApplyMotionFrame(zUIMotionFrame* frame)
{
    if (stage == Stage_Off) {
        frame->color.a = 0;
        return;
    }

    frame->offsetU = STATIC_UV_STRENGTH * (2.0f * xrand_RandomFloat() - 1.0f);
    frame->offsetV = STATIC_UV_STRENGTH * (2.0f * xrand_RandomFloat() - 1.0f);

    if (stage == Stage_FadeIn)
        // BUG: should be STATIC_FADE_IN_TIME
        frame->color.a = STATIC_ALPHA * (1.0f - timer / STATIC_FADE_OUT_TIME);
    else if (stage == Stage_FadeOut)
        frame->color.a = STATIC_ALPHA * timer / STATIC_FADE_OUT_TIME;
    else
        frame->color.a = STATIC_ALPHA;
}
