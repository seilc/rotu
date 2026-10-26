#include "xLightEffect.h"

#include "xLight.h"
#include "zBase.h"
#include "xGlobals.h"
#include "xScene.h"
#include "xRand.h"

#include <string.h>

bool xLightEffectIsValid(xBase* base)
{
    if (!base) return false;

    switch (base->baseType) {
    case eBaseTypeLightEffectFlicker:
    case eBaseTypeLightEffectStrobe:
        return true;
    default:
        return false;
    }
}

void xLightEffectFlicker::Init(xBase& data, xDynAsset& asset, size_t asset_size)
{
    xDynAssetValidate_L(48, asset, xLightEffectFlickerAsset, "effect:Light Effect Flicker");
    
    xLightEffectFlicker* flickerEffect = (xLightEffectFlicker*)&data;
    new (flickerEffect) xLightEffectFlicker();

    xBaseInit(&data, &asset);

    flickerEffect->tasset = (xLightEffectFlickerAsset*)&asset;
}

void xLightEffectFlicker::StartEffect(xLight& light, bool resetEffect)
{
    if (resetEffect) {
        xLightGetColor(&light, light.flickerData.initialColor);
        light.flickerData.time = 0.0f;
    }

    light.lightEffectRunning = true;
}

void xLightEffectFlicker::StopEffect(xLight& light, bool resetColor)
{
    light.lightEffectRunning = false;

    if (resetColor) {
        xLightSetColor(&light, light.flickerData.initialColor, 0.0f);
    }
}

void xLightEffectFlicker::UpdateEffect(xLight& light, F32 dt)
{
    xLightEffectFlickerData& flickerData = light.flickerData;

    flickerData.time -= dt;
    if (flickerData.time <= 0.0f) {
        flickerData.time = tasset->flickerRate + tasset->randomFlickerRate * xrand_GenRandFloat();

        xFColor newColor;
        newColor.a = 1.0f;
        if (tasset->flags & 0x1) {
            F32 randomNum = xrand_GenRandFloat();
            newColor.r = flickerData.initialColor.r * (1.0f - tasset->maxColorVariation.r * randomNum);
            newColor.g = flickerData.initialColor.g * (1.0f - tasset->maxColorVariation.g * randomNum);
            newColor.b = flickerData.initialColor.b * (1.0f - tasset->maxColorVariation.b * randomNum);
        } else {
            newColor.r = flickerData.initialColor.r * (1.0f - tasset->maxColorVariation.r * xrand_GenRandFloat());
            newColor.g = flickerData.initialColor.g * (1.0f - tasset->maxColorVariation.g * xrand_GenRandFloat());
            newColor.b = flickerData.initialColor.b * (1.0f - tasset->maxColorVariation.b * xrand_GenRandFloat());
        }

        F32 blendtime = xmin(tasset->blendTime, flickerData.time);

        xLightSetColor(&light, newColor, blendtime);
    }
}

void xLightEffectStrobe::Init(xBase& data, xDynAsset& asset, size_t asset_size)
{
    xDynAssetValidate_L(153, asset, xLightEffectStrobeAsset, "effect:Light Effect Strobe");

    xLightEffectStrobe* strobeEffect = (xLightEffectStrobe*)&data;
    new (strobeEffect) xLightEffectStrobe();

    xBaseInit(&data, &asset);

    strobeEffect->tasset = (xLightEffectStrobeAsset*)&asset;
    strobeEffect->tasset->minColor.a = 1.0f;
}

void xLightEffectStrobe::StartEffect(xLight& light, bool resetEffect)
{
    if (resetEffect) {
        xLightGetColor(&light, light.strobeData.maxColor);
        light.strobeData.time = 0.0f;
        light.strobeData.stage = 1;
    }

    light.lightEffectRunning = true;
}

void xLightEffectStrobe::StopEffect(xLight& light, bool resetColor)
{
    light.lightEffectRunning = false;

    if (resetColor) {
        xLightSetColor(&light, light.strobeData.maxColor, 0.0f);
    }
}

void xLightEffectStrobe::UpdateEffect(xLight& light, F32 dt)
{
    xLightEffectStrobeData& strobeData = light.strobeData;

    strobeData.time -= dt;
    if (strobeData.time <= 0.0f) {
        switch (strobeData.stage) {
        case 0:
            strobeData.stage = 1;
            strobeData.time = tasset->fadeUpTime + tasset->timeMax + tasset->randomTimeMax * xrand_GenRandFloat();
            xLightSetColor(&light, strobeData.maxColor, tasset->fadeUpTime);
            break;
        case 1:
            strobeData.stage = 0;
            strobeData.time = tasset->fadeDownTime + tasset->timeMin + tasset->randomTimeMin * xrand_GenRandFloat();
            xLightSetColor(&light, tasset->minColor, tasset->fadeDownTime);
            break;
        default:
            xFAIL_FMT_L(244, "%s", "Unknown Strobe Stage");
            break;
        }
    }
}
