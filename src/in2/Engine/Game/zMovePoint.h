#ifndef ZMOVEPOINT_H
#define ZMOVEPOINT_H

#include "xMovePoint.h"

struct zScene;

struct zMovePoint : xMovePoint
{
};

zMovePoint* zMovePoint_GetMemPool(S32 cnt, U32 memtag);
void zMovePointInit(zMovePoint* m, xMovePointAsset* asset);
zMovePoint* zMovePoint_GetInst(S32 n);
void zMovePointSetup(zMovePoint* mvpt, zScene* scn);
void zMovePointSave(zMovePoint* ent, xSerial* s);
void zMovePointLoad(zMovePoint* ent, xSerial* s);
void zMovePointReset(zMovePoint* ent);
void zMovePointEventCB(xBase* from, xBase* to, U32 toEvent, const F32* toParam, xBase* toParamWidget,
                       U32 toParamWidgetID);
xVec3* zMovePointGetPos(const zMovePoint* m);

#endif
