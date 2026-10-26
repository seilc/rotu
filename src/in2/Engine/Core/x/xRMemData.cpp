#include "xRMemData.h"

#include "xBase.h"
#include "xMemMgr.h"
#include "xDebug.h"
#include "xOutputMgr.h"

#include "decomp.h"

#include <string.h>

void* RyzMemData::operator new(size_t amt, S32 who, U32 memtag, RyzMemGrow* growCtxt)
{
    void* mem = NULL;

    xVALIDATE_L(87, who);

    if (!who) who = 'RMEM';

    xASSERT_MSG_L(89, (amt>1), "Don't be shy");

    S32 dogrow = TRUE;

    if (!growCtxt) dogrow = FALSE;
    else if (!growCtxt->IsEnabled()) dogrow = FALSE;

    if (dogrow) {
        xASSERT_MSG_L(99, growCtxt->ptr, "Missing base address");

        xOutInfo("RyzMemData",
                 "RMemGrow (0x%08X [0x%08X]) base 0x%08x add (0x%08X) %d\n",
                 growCtxt->user,
                 growCtxt->user ? growCtxt->user->id : 0,
                 growCtxt->ptr,
                 who,
                 amt);
    
        mem = xMEMGROWALLOC(amt, memtag);
    } else {
        xOutInfo("RyzMemData",
                 "RMemAlloc for [0x%08X] size %d\n",
                 who,
                 amt);
        
        mem = xMEMALLOC_L(amt, 0, memtag, who, 112);
    }

    memset(mem, 0, 4);

    return mem;
}

void RyzMemData::operator delete(void*)
{
}

DECOMP_FORCEACTIVE(
    DEBUG ? "%s(%d) : (Unexpected) in '%s'\n" : 0,
    DEBUG ? "%s(%d) : (Don't be shy) in '%s'\n" : 0
)
