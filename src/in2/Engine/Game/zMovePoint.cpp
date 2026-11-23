#include "xCommon.h"
#include "zMovePoint.h"

#include "zEvent.h"
#include "zScene.h"

static zMovePoint* g_mvpt_list;
static S32 g_mvpt_cnt;

zMovePoint* zMovePoint_GetMemPool(S32 cnt, U32 memtag)
{
    if (!cnt) g_mvpt_list = NULL;
    else
        g_mvpt_list = (zMovePoint*)xMEMALLOC(cnt * sizeof(zMovePoint), 0);

    g_mvpt_cnt = cnt;
    return g_mvpt_list;
}

void zMovePointInit(zMovePoint* m, xMovePointAsset* asset)
{
    xMovePointInit(m, asset);

    m->eventFunc = zMovePointEventCB;

    if (m->linkCount) {
        m->link = (xLinkAsset*)((U8*)asset + sizeof(xMovePointAsset) + asset->numPoints * sizeof(U32));
    } else {
        m->link = NULL;
    }
}

zMovePoint* zMovePoint_GetInst(S32 n)
{
    return &g_mvpt_list[n];
}

void zMovePointSetup(zMovePoint* mvpt, zScene* scn)
{
    xMovePointSetup(mvpt, scn);
}

void zMovePointSave(zMovePoint* ent, xSerial* s)
{
    xMovePointSave(ent, s);
}

void zMovePointLoad(zMovePoint* ent, xSerial* s)
{
    xMovePointLoad(ent, s);
}

void zMovePointReset(zMovePoint* ent)
{
    xMovePointReset(ent);
}

void zMovePointEventCB(xBase*, xBase* to, U32 toEvent, const F32*, xBase*, U32)
{
    zMovePoint* m = (zMovePoint*)to;

    switch (toEvent) {
    case eEventArrive:
        break;
    case eEventOn:
        m->on = 1;
        break;
    case eEventOff:
        m->on = 0;
        break;
    case eEventDebugReset:
    case eEventReset:
        zMovePointReset(m);
        break;
    }
}

xVec3* zMovePointGetPos(const zMovePoint* m)
{
    return xMovePointGetPos(m);
}
