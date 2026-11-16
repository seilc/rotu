#ifndef ZFREEZABLEOBJECTASSET_H
#define ZFREEZABLEOBJECTASSET_H

#include "xDynAsset.h"
#include "xVec3.h"

struct zFreezableObjectAsset : xDynAsset
{
    xVec3 pos;
    U32 modelID;
    U32 shrapnelID;
    U32 killedTargetID;
    U32 driverID;
    U32 flags;
    F32 timeToFreeze;
    F32 iceScaleMin;
};

// zFreezableObjectAsset::flags
#define k_ZFREEZABLEOBJECT_START_ON 0x1

#endif
