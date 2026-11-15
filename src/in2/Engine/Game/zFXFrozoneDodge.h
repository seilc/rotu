#ifndef ZFXFROZONEDODGE_H
#define ZFXFROZONEDODGE_H

#include "xDecal.h"
#include "xMath.h"

struct xEntFrame;
class zFrozonePlayerDodge;

class zFXFrozoneDodge
{
public:
    void Setup();
    void Begin();
    void End();
    void Update(F32 dt, xEntFrame* frame, zFrozonePlayerDodge* dodge);

private:
    bool firstUpdate;
    bool finished;
    bool blastPending;
    xResponseCurve decalResponseCurve;
    xDecal::emit_context decalContext;
    F32 decalTimer;
    U32 decalCount;
};

class zFXIceSpray
{
public:
    static void Init();
    static void Emit(const xVec3& pos, const xVec3& dir, F32 dt, F32 rate);
};

#endif
