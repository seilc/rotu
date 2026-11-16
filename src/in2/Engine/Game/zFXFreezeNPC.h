#ifndef ZFXFREEZENPC_H
#define ZFXFREEZENPC_H

#include "xDecal.h"
#include "xMath.h"

struct xEnt;
struct xModelInstance;
struct zShrapnelAsset;

#define MAX_ICE_TYPES 3

class zFXFreezeNPC
{
public:
    static bool HasBlockForEnt(xEnt* ent, U32 type);
    static void Init();
    static void Update(F32 dt);
    static void Freeze(xEnt* ent, F32 amount, U32 type);
    static void UpdateAt(xEnt* ent, U32 type);
    static void Thaw(xEnt* ent, U32 type);
    static void Release(xEnt* ent, U32 type);
    static void Shatter(xEnt* ent, U32 type);
    static void Freezing(xEnt* ent, F32 intensity);
    static void StopFreezing(xEnt* ent);
    static void Render();

private:
    struct block
    {
        F32 angle;
        xModelInstance* model;
        xEnt* ent;
        F32 amount;
        F32 emit_remaining;
        U8 unk14[4];
        xDecal::emit_context decal;
        xResponseCurve decalCurve;
    };

    struct freezing_fx
    {
        xEnt* ent;
        F32 intensity;
        F32 emit_remaining;
    };

    block* find_block(xEnt* ent, U32 type);
    freezing_fx* find_freezing_fx(xEnt* ent);
    void _Init();
    void InitDebugTweak();
    void _Update(F32 dt);
    void _Freeze(xEnt* ent, F32 amount, U32 type);
    void _UpdateAt(xEnt* ent, U32 type);
    void _Thaw(xEnt* ent, U32 type);
    void _Release(xEnt* ent, U32 type);
    void _Shatter(xEnt* ent, U32 type);
    void _Freezing(xEnt* ent, F32 intensity);
    void _StopFreezing(xEnt* ent);
    void _Render();

    zShrapnelAsset* thawShrapnel[MAX_ICE_TYPES];
    zShrapnelAsset* shatterShrapnel[MAX_ICE_TYPES];
    block* blocks[MAX_ICE_TYPES];
    freezing_fx* freezing;
    S32 frozoneIndex;
    xResponseCurve iceCurve;
};

#endif
