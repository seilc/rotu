#ifndef ZWATER_H
#define ZWATER_H

#include "xBase.h"

struct xDynAsset;
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
    bool visible;
    S32 motion_cfg_id;

public:
    static void load(xBase& data, xDynAsset& asset, size_t asset_size);
    void show();
    void hide();
    void load(const asset_type& a);
    U32 get_bodyID();
    static void cb_dispatch(xBase*, xBase* to, U32 event, const F32*, xBase*, U32);
};

#endif
