#ifndef XLIGHTEFFECTASSET_H
#define XLIGHTEFFECTASSET_H

#include "xDynAsset.h"
#include "xColor.h"

struct xLightEffectFlickerAsset : xDynAsset
{
    xFColor maxColorVariation;
    F32 flickerRate;
    F32 randomFlickerRate;
    F32 blendTime;
    U32 flags;
};

struct xLightEffectStrobeAsset : xDynAsset
{
    xFColor minColor;
    F32 fadeUpTime;
    F32 fadeDownTime;
    F32 timeMax;
    F32 timeMin;
    F32 randomTimeMax;
    F32 randomTimeMin;
};

#endif
