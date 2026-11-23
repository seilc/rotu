#include "xCommon.h"
#include "zWater.h"

#include "xDynAsset.h"
#include "zBase.h"
#include "zEvent.h"

struct zWaterBody::asset_type : xDynAsset
{
    // flags
    enum
    {
        FLAG_VISIBLE = 0x1,
        FLAG_NO_OVERFILL = 0x2
    };

    U32 flags;
    U32 motion_type;
    U32 body;
    U32 facade_refract;
    U32 facade_reflect;
    U32 light_dir;
};

void zWaterBody::load(xBase& data, xDynAsset& asset, size_t asset_size)
{
    ((zWaterBody&)data).load((asset_type&)asset);
}

void zWaterBody::show()
{
    visible = true;
}

void zWaterBody::hide()
{
    visible = false;
}

void zWaterBody::load(const asset_type& a)
{
    asset = (asset_type*)&a;
    xBaseInit(this, asset);
    baseType = eBaseTypeWaterBody;
    eventFunc = cb_dispatch;
    if (linkCount) link = (xLinkAsset*)(asset + 1);

    body_id = -1;
    visible = asset->flags & asset_type::FLAG_VISIBLE;
}

U32 zWaterBody::get_bodyID() { return asset->body; }

void zWaterBody::cb_dispatch(xBase*, xBase* to, U32 event, const F32*, xBase*, U32)
{
    zWaterBody& e = *(zWaterBody*)to;

    switch (event) {
    case eEventVisible:
    case eEventVisibilityCullOff:
        e.show();
        break;
    case eEventInvisible:
    case eEventVisibilityCullOn:
        e.hide();
        break;
    }
}
