#ifndef ZNPCCHICKENFX_H
#define ZNPCCHICKENFX_H

#include "xPtankPool.h"
#include "xVec3.h"

class zNPCChickenFXFeather
{
public:
    struct config
    {
        F32 minLife;
        F32 maxLife;
        F32 size;
        F32 fadeStart;
        U8 r, g, b;
    };

    struct particle
    {
        xVec3 pos;
        F32 size;
        xVec2 vel;
        U32 flip;
        F32 life;
        F32 fadeTime;
        F32 rot;
        F32 rotChange;
        config* cfg;
    };

    static S32 systemID;
    static bool needSetup;
    static config defaultConfig;

    static void Setup();
    static void SceneSetup();
    static void Emit(const xVec3& pos, S32 emit, F32, config* cfg);
    static S32 Update(U8* mem, S32 count, ptank_pool& pool_, F32 dt, void*);
};

#endif
