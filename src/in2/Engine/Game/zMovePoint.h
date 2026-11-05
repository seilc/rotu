#ifndef ZMOVEPOINT_H
#define ZMOVEPOINT_H

#include "xMovePoint.h"

struct zMovePoint : xMovePoint
{
};

xVec3* zMovePointGetPos(const zMovePoint* m);

#endif
