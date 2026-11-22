#include "xCommon.h"
#include "zZipLine.h"

#include "xstransvc.h"
#include "zEvent.h"
#include "zScene.h"

void zZipLineInit(void* plat, void* asset)
{
    zZipLineInit((zZipLine*)plat, (zZipLineAsset*)asset);
}

void zZipLineInit(zZipLine* zipline, zZipLineAsset* asset)
{
    xBaseInit(zipline, asset);

    zipline->asset = asset;
    zipline->eventFunc = zZipLineEventCB;

    zipline->link = (xLinkAsset*)(asset + 1);

    zipline->enabled = true;
    zipline->gotDismount = false;
}

void zZipLineSetup(zZipLine* zipline, zScene*)
{
    if (zipline->asset->boundEntityID) {
        U32 id = zipline->asset->boundEntityID;
        zipline->asset->boundEntity = (xEnt*)zSceneFindObject(id);
    }

    U32 id = zipline->asset->startSplineID;
    zipline->asset->spline = (xNurbs*)xSTFindAsset(id, NULL);
}

void zZipLineEventCB(xBase*, xBase* to, U32 toEvent, const F32*, xBase*, U32)
{
    zZipLine* zipline = (zZipLine*)to;

    switch (toEvent) {
    case eEventOn:
        zipline->enabled = true;
        break;

    case eEventOff:
        zipline->enabled = false;
        break;

    case eEventZipLineDismount:
        zipline->gotDismount = true;
        break;
    }
}
