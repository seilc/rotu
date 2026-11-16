#ifndef ZNPCASSET_H
#define ZNPCASSET_H

#include "xDynAsset.h"
#include "xEntAsset.h"

struct zNPCAsset : xDynAsset
{
    xEntAsset ent_asset;
    U32 navigation_mesh_id;
    U32 npcFlags;
    U32 respawnCounterID;
    U32 settingsHashID;
    U32 interestPointerGroupID;
    U32 lightKitID;
};

#endif
