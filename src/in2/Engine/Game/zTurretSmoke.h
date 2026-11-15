#ifndef ZTURRETSMOKE_H
#define ZTURRETSMOKE_H

#include "xColor.h"
#include "xPtankPool.h"
#include "xVec3.h"
#include "zLightWeight.h"

class zTurretSmokeSystem : public zLightweight<zTurretSmokeSystem, 10>
{
public:
    struct TurretSmokeParticle
    {
        xVec3 pos;
        F32 size;
        xVec3 vel;
        F32 rot;
        xColor color;
        F32 life;
        F32 pad;
    };

    F32 time;
    xVec3 pos;
    xVec3 dir;
    F32 emitAccum;
    F32 emitRemaining;

    static RwTexture* texture;

    static void Setup();
    zTurretSmokeSystem(const xVec3& pos, const xVec3& dir);
    void Stop();
    zTurretSmokeSystem();
    void ResetOrientation(const xVec3& pos, const xVec3& dir);
    void Update(F32 dt);
    static S32 Update(U8* mem, S32 count, ptank_pool& pool_, F32 dt, void* context);
};

#endif
