#ifndef ZFORCEFIELDEFFECT_H
#define ZFORCEFIELDEFFECT_H

#include "xMath3.h"
#include "xColor.h"

struct xVec2;
struct xModelInstance;

struct meshData
{
    xVec3* verts;
    S32 verts_size;
    U16* strip_indices;
    S32 strip_indices_size;
    F32* full_heightmap[2];
    F32* heightmap[2];
    S32 active_heightmap;
    xVec2* energy_uv;
    xColor* refract_color;
    F32* cell_width_scale;
};

class zForceFieldEffect
{
public:
    static zForceFieldEffect* GetForceField(F32 midHeight, F32 scale, const xVec3& offset, bool initalRipples);
    void FreeForceField();
    void Move(const xMat4x3& mat);
    void Update(F32 dt);
    void Impact(const xVec3& loc, F32 radius, F32 amount, bool depress);

    void SetShieldColor(const xFColor& color)
    {
        baseShieldColor = color;
        currentColor = color;
        overrideColor = 1;
    }

    void SetShieldDistortion(bool distort)
    {
        distortShield = distort;
    }

private:
    meshData mesh;
    xMat4x3 ball_mat;
    F32 energy_spin;
    F32 Aold;
    F32 Avel;
    F32 midHeight;
    S32 sphereRows;
    S32 midRows;
    xVec3 offset;
    F32 scale;
    U8 used;
    U8 skipFrame;
    U8 overrideColor;
    U8 distortShield;
    F32 lastUsedTime;
    F32 fadeTimer;
    xFColor baseShieldColor;
    xFColor currentColor;
    F32 shieldColorCycle;
    F32 shieldStrength;
    F32 timeToNextRipple;
    U8 shieldDestroyed;
    F32 arcEmitRemaining;
    xModelInstance* electricModel;
};

#endif
