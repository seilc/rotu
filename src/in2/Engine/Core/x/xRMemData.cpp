#include "xRMemData.h"

#include "xBase.h"
#include "xMemMgr.h"

#include <string.h>

void* RyzMemData::operator new(size_t amt, S32 who, U32 memtag, RyzMemGrow* growCtxt)
{
    void* mem = NULL;

    if (!who) who = 'RMEM';

    S32 dogrow = TRUE;

    if (!growCtxt) dogrow = FALSE;
    else if (!growCtxt->IsEnabled()) dogrow = FALSE;

    if (dogrow) {
        mem = xMEMGROWALLOC(amt);
    } else {
        mem = xMEMALLOC(amt, 0);
    }

    memset(mem, 0, 4);

    return mem;
}

void RyzMemData::operator delete(void*)
{
}
