#ifndef ZFXFROZONESKIS_H
#define ZFXFROZONESKIS_H

#include "types.h"

struct xModelInstance;
class zFrozonePlayer;

class zFXFrozoneSkis
{
public:
    static void SetPowerupLevel(S32 level);
    static void Init(zFrozonePlayer* player);
    static void Reset();
    static void Begin();
    static void End(bool immediate);
    static void Update(F32 dt, zFrozonePlayer* player);
    static void Render();
    static void CollisionEmit(xModelInstance* model);
};

#endif
