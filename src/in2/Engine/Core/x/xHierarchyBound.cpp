#include "xHierarchyBound.h"

#include "xDraw.h"

#include <string.h>

void xHierarchyBoundInit(xHierarchyBound* bound, const xHierarchyBoundInitData* init, F32 masterRadius, U32 extraBounds, F32 scale)
{
    U32 count = 0;
    while (init[count].bone >= -1) count++;
    bound->count = count;

    bound->nodes = (xHierarchyNode*)xMEMALLOC(sizeof(xHierarchyNode) * (count + extraBounds), 0, eMemMgrTag_HierarchyBound, 0);

    for (U32 i = 0; i < count; i++) {
        xHierarchyNode& node = bound->nodes[i];
        node.bone = init[i].bone;
        memset(&node.sphere.center, 0, sizeof(node.sphere.center));
        node.sphere.r = scale * init[i].radius;
        node.userData = init[i].userData;
    }

    memset(&bound->master.center, 0, sizeof(bound->master.center));
    bound->master.r = masterRadius * scale;
    bound->maxCount = count + extraBounds;
    memset(bound->nodes + count, 0, sizeof(xHierarchyNode) * extraBounds);
}

void xHierarchyBoundDraw(const xHierarchyBound* bound, U8 mR, U8 mG, U8 mB, U8 mA, U8 sR, U8 sG, U8 sB, U8 sA, S8 hilight, U8 hR, U8 hG, U8 hB, U8 hA)
{
    xDrawSetColor(mR, mG, mB, mA);
    xDrawSphere(&bound->master, 0xC0006);

    xDrawSetColor(sR, sG, sB, sA);
    for (U32 i = 0; i < bound->count; i++) {
        if ((S32)i == hilight) {
            xDrawSetColor(hR, hG, hB, hA);
        }
        xDrawSphere(&bound->nodes[i].sphere, 0xC0006);
        if ((S32)i == hilight) {
            xDrawSetColor(sR, sG, sB, sA);
        }
    }
}

bool xHierarchyBoundTest(const xHierarchyBound* bound, xSweptSphere* sphere, U16* boundUserData, S8* subsphere, bool skipMasterCheck)
{
    if (!skipMasterCheck && !xSweptSphereToSphere(sphere, &bound->master)) {
        return false;
    }

    xSweptSphereResetCollision(sphere);

    for (U32 i = 0; i < bound->count; i++) {
        if (xSweptSphereToSphere(sphere, &bound->nodes[i].sphere)) {
            if (boundUserData) {
                *boundUserData = bound->nodes[i].userData;
            }
            if (subsphere) {
                *subsphere = i;
            }
            return true;
        }
    }

    return false;
}
