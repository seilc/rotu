#include "xVolume.h"

#include "xVolumeAsset.h"

void xVolume::Init(xVolumeAsset* asset)
{
    xBaseInit(this, asset);

    this->asset = asset;

    if (this->linkCount)
        this->link = (xLinkAsset*)((U8*)this->asset + sizeof(xVolumeAsset));
    else
        this->link = NULL;
}

void xVolume::Reset()
{
    xBaseReset(this, this->asset);
}

void xVolume::Save(xSerial* s)
{
    xBaseSave(this, s);
}

void xVolume::Load(xSerial* s)
{
    xBaseLoad(this, s);
}

xBound* xVolume::GetBound()
{
    return &this->asset->bound;
}
