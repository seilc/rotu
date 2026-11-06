#ifndef XMEMMGR_H
#define XMEMMGR_H

#include "iMemMgr.h"

#include <stddef.h>
#include <new>

struct xMemPool;
typedef void(*xMemPoolInitCallback)(xMemPool*, void*);

struct xMemPool
{
    void* FreeList;
    U16 NextOffset;
    U16 Flags;
    void* UsedList;
    xMemPoolInitCallback InitCB;
    void* Buffer;
    U16 Size;
    U16 NumRealloc;
    U32 Total;
};

enum eMemMgrTag
{
    eMemMgrTag_2D,
    eMemMgrTag_Anim,
    eMemMgrTag_Audio,
    eMemMgrTag_Bomb,
    eMemMgrTag_Camera,
    eMemMgrTag_Cinematic,
    eMemMgrTag_Destructible,
    eMemMgrTag_Debug,
    eMemMgrTag_Decal,
    eMemMgrTag_Duplicator,
    eMemMgrTag_Event,
    eMemMgrTag_FMV,
    eMemMgrTag_FXActionLine,
    eMemMgrTag_FXRibbon,
    eMemMgrTag_Enemy,
    eMemMgrTag_EnergyStream,
    eMemMgrTag_EntityMisc,
    eMemMgrTag_EnvSpatial,
    eMemMgrTag_EnvAbstract,
    eMemMgrTag_Explosion,
    eMemMgrTag_Flame,
    eMemMgrTag_FlameThrower,
    eMemMgrTag_ForceFieldEffect,
    eMemMgrTag_FrozenObject,
    eMemMgrTag_Grass,
    eMemMgrTag_HierarchyBound,
    eMemMgrTag_HUD,
    eMemMgrTag_IncrediSlam,
    eMemMgrTag_Im3DHelper,
    eMemMgrTag_LaserBeam,
    eMemMgrTag_LaserBolt,
    eMemMgrTag_Light,
    eMemMgrTag_Lightning,
    eMemMgrTag_Lightweight,
    eMemMgrTag_Message,
    eMemMgrTag_Model,
    eMemMgrTag_MotionBlur,
    eMemMgrTag_Movable,
    eMemMgrTag_MovePoint,
    eMemMgrTag_NavigationMesh,
    eMemMgrTag_NPCFX,
    eMemMgrTag_OneLiner,
    eMemMgrTag_OrdArrayMgr,
    eMemMgrTag_Packer,
    eMemMgrTag_PackerAssetPool,
    eMemMgrTag_PackerDefault,
    eMemMgrTag_PackerTOC,
    eMemMgrTag_PackerUnknown,
    eMemMgrTag_Particle,
    eMemMgrTag_Physics,
    eMemMgrTag_Player,
    eMemMgrTag_Projectile,
    eMemMgrTag_RenderBuffer,
    eMemMgrTag_Rubble,
    eMemMgrTag_Scene,
    eMemMgrTag_ScreenFX,
    eMemMgrTag_Serializer,
    eMemMgrTag_Shadow,
    eMemMgrTag_Shower,
    eMemMgrTag_SmashRing,
    eMemMgrTag_SoundRAMAsset,
    eMemMgrTag_Splash,
    eMemMgrTag_Spline,
    eMemMgrTag_SpotLight,
    eMemMgrTag_Subtitle,
    eMemMgrTag_Texture,
    eMemMgrTag_TextureStream,
    eMemMgrTag_Throwable,
    eMemMgrTag_ThrowableSystem,
    eMemMgrTag_Trigger,
    eMemMgrTag_UberLaser,
    eMemMgrTag_UI,
    eMemMgrTag_UpdateCullMgr,
    eMemMgrTag_VideoRAMAsset,
    eMemMgrTag_ReactiveAnim,
    eMemMgrTag_RenderWare,
    eMemMgrTag_Shrapnel,
    eMemMgrTag_Water,
    eMemMgrTag_WaterHose,
    eMemMgrTag_AnimCustom,
    eMemMgrTag_SwitchLever,
    eMemMgrTag_PlayerFX,
    eMemMgrTag_RenderWareMisc,
    eMemMgrTag_DecalRWMalloc,
    eMemMgrTag_Heists,
    eMemMgrTag_MiniGame,
    eMemMgrTag_CookingMinigame,
    eMemMgrTag_NumTags
};

// Not in DWARF data
enum xMemStaticType
{
    eMemStaticType_0, // unknown
};

struct xMemArea
{
    U32 addr;
    U32 size;
    U32 flags;
};

struct xMemInfo
{
    xMemArea system;
    xMemArea stack;
    xMemArea DRAM;
};

#define XMEMMAXSTACK 12

enum { XMEM_MAX_HEAPS = 2 };

typedef void(*xMemFreeFunc)(void*);
typedef void(*xMemBaseNotifyFunc)();

struct xMemFreeLink
{
    xMemFreeLink* next;
    xMemFreeFunc func;
    void* param;
};

struct xHeapState
{
    U32 curr;
    U16 blk_ct;
    U16 pad;
    U32 used;
    U32 wasted;
    xMemFreeLink* free_funcs;
};

struct xMemBlock
{
    U32 addr;
    U32 size;
    S32 align;
};

struct xMemHeap
{
    U32 flags;
    U32 hard_base;
    U32 size;
    S16 opp_heap[2];
    xHeapState state[XMEMMAXSTACK];
    U16 state_idx;
    U16 max_blks;
    xMemBlock* blk;
    xMemBlock* lastblk;
};

extern xMemInfo gMemInfo;
extern U32 gActiveHeap;
extern xMemHeap gxHeap[XMEM_MAX_HEAPS];
extern U32 scratch_mem_used;

#define xMEMGROWALLOC(size) xMemGrowAlloc(gActiveHeap, (size))

#define xMEMALLOC(size, align) xMemAlloc(gActiveHeap, (size), (align))

#define xMEMPUSHTEMP(amt) xMemPushTemp((amt))

#define xMEMPOPTEMP(pointer) iMemPopTemp((pointer))

void xMemDebug_SoakLog(U32 SceneId);
void xMemInit();
void xMemExit();
void xMemInitHeap(xMemHeap* heap, U32 base, U32 size, U32 flags);

S32 xMemPushBase(U32 heapID);
S32 xMemPushBase();
void* xMemAllocFreeable(void* alloced_memory, S32 asked_for_size, xMemFreeFunc func, void* param);
S32 xMemPopBase(U32 heapID, S32 depth);
S32 xMemPopBase(S32 depth);
S32 xMemGetBase(U32 heapID);
void xMemRegisterBaseNotifyFunc(xMemBaseNotifyFunc func);
S32 xMemGetBase();
void xMemPoolSetup(xMemPool* pool, void* buffer, U32 nextOffset, U32 flags, xMemPoolInitCallback initCB, U32 size, U32 count, U32 numRealloc);
void* xMemPoolAlloc(xMemPool* pool, U32 memtag);
void xMemPoolFree(xMemPool* pool, void* data);
void* xMemScratchPush(U32 size, U32 align);
void xMemScratchPop(void* mem);

void* xMemGrowAlloc(U32 heapID, U32 size);

void* xMemAlloc(U32 heapID, U32 size, S32 align);

inline void* xMemPushTemp(U32 amt)
{
    return iMemPushTemp(amt);
}

inline void* operator new(size_t size, xMemStaticType, U32 tag, U32 assetID = 0)
{
    return xMEMALLOC(size, 0);
}

inline void* operator new[](size_t size, xMemStaticType, U32 tag, U32 assetID = 0)
{
    return xMEMALLOC(size, 0);
}

#endif
