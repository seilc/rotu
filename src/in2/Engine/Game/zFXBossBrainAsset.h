#ifndef ZFXBOSSBRAINASSET_H
#define ZFXBOSSBRAINASSET_H

#include "xDynAsset.h"
#include "xVec3.h"

struct zFXBossBrainAsset : xDynAsset
{
    U32 flags;
    xVec3 pos;
    U32 attach_id;
    S32 attach_bone;
    S32 num_cards;
    F32 card_time;
    F32 scale;
    U32 raster_id;
    U32 dome_id;
};

// zFXBossBrainAsset::flags
#define k_ZFXBOSSBRAIN_START_ON 0x1

#endif
