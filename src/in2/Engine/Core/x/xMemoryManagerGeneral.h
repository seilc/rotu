#ifndef XMEMORYMANAGERGENERAL_H
#define XMEMORYMANAGERGENERAL_H

#include "xMemoryManager.h"

class xMemoryManagerGeneral : public xMemoryManager
{
public:
    enum Dir
    {
        Up,
        Down
    };

    enum Strategy
    {
        FirstFit,
        BestFit
    };

    xMemoryManagerGeneral();

    void Init(void* start, U32 size, Dir dir, Strategy strategy, bool debugging);
    void* GetCurrentEnd() const;

protected:
#if DEBUG || RELEASE
    virtual void* DoAllocate(U32 size, U32 options, const char*, const char*, S32);
#else
    virtual void* DoAllocate(U32 size, U32 options);
#endif

    virtual void DoFree(void* pointer);

#if DEBUG || RELEASE
    virtual void* DoReallocate(void* pointer, U32 size, U32 options, const char* file, const char* function, S32 line);
#else
    virtual void* DoReallocate(void* pointer, U32 size, U32 options);
#endif

    virtual U32 DoGetBlockSize(void* pointer) const;

private:
    struct FreeNode
    {
        FreeNode* next;
        FreeNode* prev;
        U32 size;
        U32 pad[1];
    };

    struct AllocatedNode
    {
        U32 size;
        U32 pad[3];
    };

    FreeNode* freeStart;
    FreeNode* freeEnd;
    Dir dir;
    Strategy strategy;

    void MergeNodes(FreeNode* node);
    AllocatedNode* ShortenNode(AllocatedNode* allocatedNode, U32 size);

    FreeNode* NextBlockOfMemory(FreeNode* node) const
    {
        return dir == Up ?
            (FreeNode*)((U8*)(node + 1) + node->size) :
            (FreeNode*)((U8*)(node - 1) - node->size);
    }
};

#endif
