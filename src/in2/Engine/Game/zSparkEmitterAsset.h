#ifndef ZSPARKEMITTERASSET_H
#define ZSPARKEMITTERASSET_H

#include "xColor.h"
#include "xDynAsset.h"
#include "xVec3.h"

struct glintInfo
{
    U32 texture;
    F32 length_min;
    F32 length_rand;
    F32 length_max;
};

struct lightInfo
{
    xFColor lightColor;
    F32 radius;
    F32 up;
    F32 max;
    F32 down;
};

struct zSparkEmitterAsset : xDynAsset
{
    F32 update_distance;
    U32 texture;
    U32 flags;
    U32 attach_obj_id;
    xVec3 position;
    S32 num_sparks;
    F32 period;
    F32 period_rand;
    F32 lifetime_min;
    F32 lifetime_rand;
    F32 yaw;
    F32 pitch;
    F32 dir_vary;
    F32 velocity_min;
    F32 velocity_rand;
    F32 gravity;
    F32 size_min;
    F32 size_rand;
    glintInfo glint;
    lightInfo light;
    U32 soundAsset;

    void SetDefaults();
};

#endif
