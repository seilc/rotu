#include "xParMgr.h"

#include "xPar.h"
#include "iParMgr.h"

static U32 sFrameCount;
static S32 sParMgrInit;

void xParMgrInit()
{
    if (!sParMgrInit) {
        xParMemInit();
        iParMgrInit();
        sParMgrInit = TRUE;
    } else {
        xParMgrKillAllParticles();
        iParMgrInit();
    }
}

void xParMgrKillAllParticles()
{
}

void xParMgrUpdate(F32 elapsedTime)
{
    U32 numFrames = 1;

    iParMgrUpdate(elapsedTime);
    numFrames = 1;

    sFrameCount += numFrames;

    if (sFrameCount > 10)
        sFrameCount = 1;

    while (sFrameCount >= 1) {
        sFrameCount--;
    }
}
