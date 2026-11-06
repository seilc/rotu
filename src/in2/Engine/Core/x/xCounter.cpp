#include "xCounter.h"

#include "xstransvc.h"
#include "xserializer.h"
#include "xDebugTweak.h"
#include "zBase.h"
#include "zEvent.h"
#include "zUICustomLevelManager.h"
#include "xFont.h"
#include "xScene.h"

#include "decomp.h"

#include <string.h>

namespace {

void add_tweaks(xCounter& counter)
{
}

}

void xCounterInit()
{
    xTWEAK_REMOVE("Widgets|Counters");
}

void xCounterInit(void* b, void* asset)
{
    xCounterInit((xBase*)b, (xCounterAsset*)asset);
}

void xCounterInit(xBase* b, xCounterAsset* asset)
{
    xCounter* t = (xCounter*)b;

    xBaseInit(b, asset);

    t->eventFunc = xCounterEventCB;
    t->asset = asset;

    if (t->linkCount) {
        t->link = (xLinkAsset*)((U8*)t->asset + sizeof(xCounterAsset));
    } else {
        t->link = NULL;
    }

    t->state = 0;
    t->count = asset->count;

    add_tweaks(*t);
}

void xCounterReset(xBase* b)
{
    xCounter* t = (xCounter*)b;

    xBaseInit(t, t->asset);

    if (t->linkCount) {
        t->link = (xLinkAsset*)((U8*)t->asset + sizeof(xCounterAsset));
    } else {
        t->link = NULL;
    }

    t->count = t->asset->count;
    t->state = 0;
}

void xCounterSave(xCounter* ent, xSerial* s)
{
    xBaseSave(ent, s);

    s->Write(ent->state);
    s->Write(ent->count);
}

void xCounterLoad(xCounter* ent, xSerial* s)
{
    xBaseLoad(ent, s);

    s->Read(&ent->state);
    s->Read(&ent->count);
}

void xCounterEventCB(xBase*, xBase* to, U32 toEvent, const F32* toParam, xBase* toParamWidget, U32)
{
    xCounter* t = (xCounter*)to;

    switch (toEvent) {
    case eEventDecrement:
        t->count--;
        if (t->count <= 0) {
            zEntEvent(t, t, eEventExpired);
        } else if (t->count > 0 && t->count <= 20) {
            zEntEvent(t, t, eEventCount1 + (U32)(t->count - 1));
        }
        break;
    case eEventIncrement:
        t->count++;
        if (t->count <= 0) {
            zEntEvent(t, t, eEventExpired);
        } else if (t->count > 0 && t->count <= 20) {
            zEntEvent(t, t, eEventCount1 + (U32)(t->count - 1));
        }
        break;
    case eEventReset:
    case eEventDebugReset:
        t->state = 0;
        t->count = t->asset->count;
        break;
    case eEventExpired:
        t->count = 0;
        t->state = 1;
        break;
    case eEventSetCount:
        if (toParamWidget && toParamWidget->baseType == eBaseTypeCounter) {
            xCounter* paramCounter = (xCounter*)toParamWidget;
            t->count = paramCounter->count;
        } else {
            t->count = toParam[0];
        }
        break;
    case eEventSetCounterToCurrentLevel:
        t->count = zUIIncrediblesGetCurrentSceneNumber();
        break;
    case eEventSetCounterToLevelStatus:
    {
        U32 level = ((xCounter*)toParamWidget)->count;
        if (zUIIncrediblesSceneIsCompleted(level)) {
            t->count = 2;
        } else if (zUIIncrediblesSceneIsUnlocked(level)) {
            t->count = 1;
        } else {
            t->count = 0;
        }
        break;
    }
    default:
        if (toEvent >= eEventCount1 && toEvent <= eEventCount20) {
            t->count = toEvent - (eEventCount1 - 1);
        } else if (toEvent == eEventCount0) {
            t->count = 0;
        }
        break;
    }
}

DECOMP_FORCEACTIVE(
    "Counter",
    "%d", "%.*s",
    &xtextbox::jot::reset_flags
)
