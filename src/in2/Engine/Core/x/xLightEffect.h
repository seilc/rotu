#ifndef XLIGHTEFFECT_H
#define XLIGHTEFFECT_H

#include "xBase.h"
#include "xLightEffectAsset.h"

struct xLight;

class xLightEffectBase : public xBase
{
public:
    virtual void StartEffect(xLight& light, bool resetEffect) = 0;
    virtual void StopEffect(xLight& light, bool resetColor) = 0;
    virtual void UpdateEffect(xLight& light, F32 dt) = 0;
};

class xLightEffectFlicker : xLightEffectBase
{
public:
    static void Init(xBase& data, xDynAsset& asset, size_t asset_size);

    virtual void StartEffect(xLight& light, bool resetEffect);
    virtual void StopEffect(xLight& light, bool resetColor);
    virtual void UpdateEffect(xLight& light, F32 dt);

private:
    xLightEffectFlickerAsset* tasset;
};

class xLightEffectStrobe : xLightEffectBase
{
public:
    static void Init(xBase& data, xDynAsset& asset, size_t asset_size);

    virtual void StartEffect(xLight& light, bool resetEffect);
    virtual void StopEffect(xLight& light, bool resetColor);
    virtual void UpdateEffect(xLight& light, F32 dt);

private:
    xLightEffectStrobeAsset* tasset;
};

#endif
