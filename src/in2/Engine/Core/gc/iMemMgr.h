#ifndef IMEMMGR_H
#define IMEMMGR_H

#include "types.h"

void iMemInit();
void iMemExit();
bool iMemMgrCheckCollisionWithMallocHeap(void* mem, U32 size);
void iMemMgrEnableRenderWareMemoryHack(bool hackEnabled);
void* iMemPushTemp(U32 size);
void iMemPopTemp(void* pointer);

#endif
