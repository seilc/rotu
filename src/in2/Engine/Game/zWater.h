#ifndef ZWATER_H
#define ZWATER_H

#include "xBase.h"

struct xEnt;

class zWaterBody : private xBase
{
public:
    struct asset_type;

    S32 motion_type;

private:
    asset_type* asset;
    xEnt* body_ent;
    xEnt** refract_ents;
    S32 refract_ents_size;
    xEnt** reflect_ents;
    S32 reflect_ents_size;
    S32 body_id;
    U8 visible;
    S32 motion_cfg_id;

public:
    U32 get_bodyID();
};

#endif
