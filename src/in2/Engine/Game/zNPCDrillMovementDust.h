#ifndef ZNPCDRILLMOVEMENTDUST_H
#define ZNPCDRILLMOVEMENTDUST_H

#include "xDecal.h"
#include "xMath3.h"
#include "zTransientModel.h"

struct xEnt;
struct ptank_pool;

namespace zFX
{

class zNPCDrillMovementDust
{
public:
    struct config
    {
        F32 min_life;
        F32 max_life;
        F32 min_size;
        F32 max_size;
        F32 velocity_min;
        F32 velocity_max;
        xVec3 velocity_dir_scale;
        F32 emit_rate;
        F32 slow;
        F32 fade_start;
        F32 gravity;
        U8 r;
        U8 g;
        U8 b;
        U8 cinematic_only;
        F32 system_emit_time;
        F32 glow;
        F32 min_rot;
        F32 max_rot;
        F32 size_delta;
        xVec3 start_velocity;
    };

    struct particle
    {
        xVec3 pos;
        F32 size;
        xVec3 vel;
        F32 life;
        F32 rot;
        F32 rot_delta;
        S32 texture_index;
        config* cfg;
    };

    static void setup();
    static void scene_setup();
    static void Reset();
    static void EmitDebris(xEnt* ent, const xVec3& from, const xVec3& to, F32& emit_remaining, bool supersize);
    static void EmitDebris(const xVec3& at, const xVec3& right, const xVec3& up, const xVec3& from, const xVec3& to,
                           F32& emit_remaining, U32 trail, bool supersize);
    static void emit(const xVec3& position, const xVec3& normal, F32& emit_remaining, const xVec3& velocity, F32 dt,
                     config* cfg, S32 type);
    static void _emit(const xVec3& position, const xVec3& normal, F32& emit_remaining, const xVec3& velocity, F32 dt,
                      config* cfg, U32 type);
    static S32 update(U8* mem, S32 count, ptank_pool& pool_, F32 dt, void*);

    static config default_config;
    static S32 dust_system_id;
    static S32 clod_system_id;
    static bool need_setup;
    static xDecal::emit_context trailDecal;
    static xResponseCurve trailDecalResponseCurve;
    static zTransientModelEmitter debrisEmitter;
};

} // namespace zFX

#endif
