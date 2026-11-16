#ifndef ZSPARKEMITTER_H
#define ZSPARKEMITTER_H

#include "xBase.h"
#include "xSnd.h"
#include "xVec3.h"
#include "zSparkEmitterAsset.h"

class zSparkEmitter : public xBase
{
public:
    static bool EmitSparks(const zSparkEmitterAsset& emitData, const xVec3& pos, const xVec3& dir, F32 dt,
                           S32 sparkSystemId, S32 glintSystemId, xSndGroupHandle sound);
};

#endif
