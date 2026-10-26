#ifndef XHIERARCHYBOUND_H
#define XHIERARCHYBOUND_H

#include "xMath3.h"
#include "xCollideSweptSphere.h"

struct xHierarchyNode
{
    xSphere sphere;
    U16 userData;
    S8 bone;
};

struct xHierarchyBound
{
    xSphere master;
    xHierarchyNode* nodes;
    U8 count;
    U8 maxCount;
};

struct xHierarchyBoundInitData
{
    S8 bone;
    F32 radius;
    U16 userData;
};

void xHierarchyBoundInit(xHierarchyBound* bound, const xHierarchyBoundInitData* init, F32 masterRadius, U32 extraBounds, F32 scale);
void xHierarchyBoundDraw(const xHierarchyBound* bound, U8 mR, U8 mG, U8 mB, U8 mA, U8 sR, U8 sG, U8 sB, U8 sA, S8 hilight, U8 hR, U8 hG, U8 hB, U8 hA);
bool xHierarchyBoundTest(const xHierarchyBound* bound, xSweptSphere* sphere, U16* boundUserData, S8* subsphere, bool skipMasterCheck);

#endif
