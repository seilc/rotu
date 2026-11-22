#ifndef ZZIPLINE_H
#define ZZIPLINE_H

#include "xBase.h"
#include "zZipLineAsset.h"

struct zScene;

struct zZipLine : xBase
{
    zZipLineAsset* asset;
    bool enabled;
    bool gotDismount;
};

void zZipLineInit(void* plat, void* asset);
void zZipLineInit(zZipLine* zipline, zZipLineAsset* asset);
void zZipLineSetup(zZipLine* zipline, zScene*);
void zZipLineEventCB(xBase*, xBase* to, U32 toEvent, const F32*, xBase*, U32);

#endif
