#include "xMemoryManagerFixed.h"

#include "xMemMgr.h"

void xMemoryManagerFixed::Init(void* start, U32 elements, U32 elementSize)
{
    xMEMORYMANAGER_ASSERT_L(89, elementSize > 0 && elementSize >= sizeof(FixedHeader) && elements > 0);

    DoInit(start, elements * elementSize, false);

    this->elements = elements;
    this->elementSize = elementSize;

    InitMemory();
}

#if DEBUG || RELEASE
void* xMemoryManagerFixed::DoAllocate(U32 size, U32 options, const char*, const char*, S32)
#else
void* xMemoryManagerFixed::DoAllocate(U32 size, U32 options)
#endif
{
    xASSERT_L(108, (options & xMEMORYOPT_ALIGN_MASK) >> xMEMORYOPT_ALIGN_SHIFT <= 2);

    if (!freeList) return NULL;

    xASSERT_L(115, size <= elementSize);

    FixedHeader* ret = freeList;

    freeList = freeList->next;

    return ret;
}

void xMemoryManagerFixed::DoFree(void* pointer)
{
    S32 TESTVALUE = (S32)((S32)pointer - (S32)GetArenaStart());
    xASSERT_L(138, TESTVALUE % elementSize == 0);

    FixedHeader* header = (FixedHeader*)pointer;
    header->next = freeList;
    this->freeList = header;
}

#if DEBUG || RELEASE
void* xMemoryManagerFixed::DoReallocate(void* pointer, U32 size, U32, const char*, const char*, S32)
#else
void* xMemoryManagerFixed::DoReallocate(void* pointer, U32 size, U32)
#endif
{
    xASSERT_L(158, size <= elementSize);
    return pointer;
}

U32 xMemoryManagerFixed::DoGetBlockSize(void*) const
{
    return elementSize;
}

void xMemoryManagerFixed::InitMemory()
{
    freeList = (FixedHeader*)GetArenaStart();

    FixedHeader* cur = (FixedHeader*)freeList;
    for (U32 i = 0; i < elements - 1; i++) {
        cur->next = xMEMADVANCE(cur, elementSize);
        cur = cur->next;
    }
    cur->next = NULL;
}
