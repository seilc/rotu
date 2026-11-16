#include "xCommon.h"
#include "zNPCDrillMovementDust.h"

#include "xCam.h"
#include "xEnt.h"
#include "xGlobals.h"
#include "xMath.h"
#include "xModel.h"
#include "xParticleBatch.h"
#include "xPtankPool.h"
#include "xRand.h"
#include "xSpacePartitionTree.h"
#include "xString.h"
#include "xstransvc.h"
#include "xutil.h"
#include "zGlobals.h"
#include "zHashes.h"
#include "zNPCCommon.h"
#include "zNPCLevelSpecific.h"

#include <string.h>

bool zFX::zNPCDrillMovementDust::need_setup;

namespace zFX
{

static const U32 debrisModels[] = { HASH_fx_drills_trail };
static const U32 debrisModelsIcy[] = { HASH_fx_drills_trail_icy };
static const U32 debrisModelsMetal[] = { HASH_fx_drills_trail_metal };

zTransientModelEmitter::config_type debrisConfig = {
    96, 4, 0.5f, 50.0f, 0.2f, 0.5f, 0.0f, 1.0f, 1.25f, 0.8f, 1.0f, { 1.0f, 1.0f, 1.0f, 1.0f },
    debrisModels, ARRAY_LENGTH(debrisModels)
};
zTransientModelEmitter::config_type debrisConfigSupersize = {
    96, 4, 0.5f, 50.0f, 0.05f, 0.5f, 0.0f, 2.0f, 2.5f, 0.8f, 1.0f, { 1.0f, 1.0f, 1.0f, 1.0f },
    debrisModels, ARRAY_LENGTH(debrisModels)
};

static zNPCDrillMovementDust::config sNPCDrillerMovementClodsConfig;
static F32 s_clods_emit_remaining;

} // namespace zFX

zFX::zNPCDrillMovementDust::config zFX::zNPCDrillMovementDust::default_config;
xDecal::emit_context zFX::zNPCDrillMovementDust::trailDecal;
xResponseCurve zFX::zNPCDrillMovementDust::trailDecalResponseCurve;
zTransientModelEmitter zFX::zNPCDrillMovementDust::debrisEmitter;

void zFX::zNPCDrillMovementDust::setup()
{
    need_setup = true;

    default_config.min_life = 1.0f;
    default_config.max_life = 1.0f;
    default_config.min_size = 0.2f;
    default_config.max_size = 0.2f;
    default_config.velocity_min = 50.0f;
    default_config.velocity_max = 50.0f;
    default_config.velocity_dir_scale.assign(1.0f, 1.0f, 1.0f);
    default_config.emit_rate = 300.0f;
    default_config.gravity = 0.1f;
    default_config.slow = 500.0f;
    default_config.fade_start = 0.2f;
    default_config.gravity = 0.0f;
    default_config.r = 255;
    default_config.g = 255;
    default_config.b = 255;
    default_config.cinematic_only = 0;
    default_config.system_emit_time = 0.1f;
    default_config.glow = 0.2f;
    default_config.min_rot = -5.0f;
    default_config.max_rot = 5.0f;
    default_config.size_delta = 2.0f;
    default_config.start_velocity.assign(0.0f, 0.0f, 0.0f);

    sNPCDrillerMovementClodsConfig = default_config;
    sNPCDrillerMovementClodsConfig.emit_rate = 10.0f;
    sNPCDrillerMovementClodsConfig.fade_start = 0.0f;
    sNPCDrillerMovementClodsConfig.glow = 1.0f;
    sNPCDrillerMovementClodsConfig.max_life = 0.8f;
    sNPCDrillerMovementClodsConfig.min_life = 0.8f;
    sNPCDrillerMovementClodsConfig.max_size = 0.3f;
    sNPCDrillerMovementClodsConfig.min_size = 0.2f;
    sNPCDrillerMovementClodsConfig.size_delta = 0.0f;
    sNPCDrillerMovementClodsConfig.slow = 0.0f;
    sNPCDrillerMovementClodsConfig.velocity_min = 4.0f;
    sNPCDrillerMovementClodsConfig.velocity_max = 5.0f;
    sNPCDrillerMovementClodsConfig.gravity = 15.0f;
    sNPCDrillerMovementClodsConfig.start_velocity.assign(0.0f, 4.0f, 0.0f);
    sNPCDrillerMovementClodsConfig.velocity_dir_scale.assign(1.0f, 0.3f, 1.0f);

    s_clods_emit_remaining = 0.0f;

    if (zNPCLevelSpecificIsIceLevel()) {
        debrisConfig.models = debrisModelsIcy;
    }

    if (zNPCLevelSpecificIsMetalLevel()) {
        debrisConfig.models = debrisModelsMetal;
    }

    if (zNPCLevelSpecificIsDirtLevel()) {
        debrisConfig.models = debrisModels;
    }

    debrisEmitter.create();
    debrisEmitter.reset();

    U32 actual_id;
    if (xModelFindAtomic(debrisConfig.models[0], &actual_id) != NULL) {
        debrisEmitter.activate(debrisConfig);
    }
}

S32 zFX::zNPCDrillMovementDust::dust_system_id = -1;
S32 zFX::zNPCDrillMovementDust::clod_system_id = -1;

void zFX::zNPCDrillMovementDust::scene_setup()
{
    static const xDecal::curve_node decalCurve[] = {
        { 0.0f, { 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f } },
        { 0.04f, { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f } },
        { 0.9f, { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f } },
        { 1.0f, { 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f } }
    };

    memset(&trailDecal, 0, sizeof(trailDecal));
    trailDecalResponseCurve.reset(sizeof(xDecal::animix) / sizeof(F32), decalCurve, ARRAY_LENGTH(decalCurve));

    trailDecal.flags = xDecal::emit_context::FLAG_ANIM_COLOR | xDecal::emit_context::FLAG_ANIM_UV;
    trailDecal.blend_mode = xDecal::BLEND_MODE_AVERAGE;
    trailDecal.uv[0].assign(0.0f, 0.0f);
    trailDecal.uv[1].assign(1.0f, 1.0f);

    if (zNPCLevelSpecificIsIceLevel()) {
        trailDecal.texture = (RwTexture*)xSTFindAsset(xStrHash("fx_drills_trail_decal_icy"), NULL);
    }

    if (zNPCLevelSpecificIsMetalLevel()) {
        trailDecal.texture = (RwTexture*)xSTFindAsset(xStrHash("fx_drills_trail_decal_metal"), NULL);
    }

    if (zNPCLevelSpecificIsDirtLevel()) {
        trailDecal.texture = (RwTexture*)xSTFindAsset(xStrHash("fx_drills_trail_decal"), NULL);
    }

    trailDecal.curve = &trailDecalResponseCurve;
    trailDecal.life = 5.0f;
    trailDecal.max_dist = 3.0f;
    trailDecal.size.assign(1.5f, 1.5f);

    RwTexture* texture = NULL;

    if (zNPCLevelSpecificIsIceLevel()) {
        texture = (RwTexture*)xSTFindAsset(xStrHash("fx_driller_dust_icy"), NULL);
    }

    if (zNPCLevelSpecificIsMetalLevel()) {
        texture = (RwTexture*)xSTFindAsset(xStrHash("fx_driller_dust_metal"), NULL);
    }

    if (zNPCLevelSpecificIsDirtLevel()) {
        texture = (RwTexture*)xSTFindAsset(xStrHash("fx_driller_dust"), NULL);
    }

    xParticleBatchSystem batch_system;
    batch_system.context = NULL;
    batch_system.update = update;
    batch_system.pool_type = PGT_POS_COLOR_SIZE_ROT_UV2;
    batch_system.rs.flags = 0;
    batch_system.rs.src_blend = rwBLENDSRCALPHA;
    batch_system.rs.dst_blend = rwBLENDINVSRCALPHA;
    batch_system.rs.texture = texture;
    batch_system.stride = sizeof(particle);

    dust_system_id = xPARTICLEBATCHADDSYSTEM(batch_system);

    if (zNPCLevelSpecificIsIceLevel()) {
        texture = (RwTexture*)xSTFindAsset(xStrHash("fx_driller_clods_icy"), NULL);
    }

    if (zNPCLevelSpecificIsMetalLevel()) {
        texture = (RwTexture*)xSTFindAsset(xStrHash("fx_driller_clods_metal"), NULL);
    }

    if (zNPCLevelSpecificIsDirtLevel()) {
        texture = (RwTexture*)xSTFindAsset(xStrHash("fx_driller_clods"), NULL);
    }

    batch_system.rs.texture = texture;

    clod_system_id = xPARTICLEBATCHADDSYSTEM(batch_system);
}

void zFX::zNPCDrillMovementDust::Reset()
{
    s_clods_emit_remaining = 0.0f;

    debrisEmitter.reset();

    U32 actual_id;
    if (xModelFindAtomic(debrisConfig.models[0], &actual_id) != NULL) {
        debrisEmitter.activate(debrisConfig);
    }
}

void zFX::zNPCDrillMovementDust::EmitDebris(xEnt* ent, const xVec3& from, const xVec3& to, F32& emit_remaining,
                                            bool supersize)
{
    if (ent->model->Anim->Single->Time == 0.0f) {
        return;
    }

    xVec3 dir;
    dir.Sub(to, from);
    F32 dist = dir.length();

    if (!(dist > 0.7f * emit_remaining)) {
        emit_remaining -= dist / 0.7f;
        return;
    }

    dir.safe_normalize(*xEntGetFacingDirection(ent));

    xVec3 up = *xEntGetUpDirection(ent);
    up.AddScale(dir, -up.dot(dir));
    up.safe_normalize(*xEntGetUpDirection(ent));

    if (need_setup) {
        need_setup = false;
        scene_setup();
    }

    xVec3 pos = from;

    if (((zNPC::zNPCCommon*)ent)->commonFlags.firstUpdate) {
        return;
    }

    while (dist > 0.7f * emit_remaining) {
        pos.AddScale(dir, 0.7f * emit_remaining);
        dist -= 0.7f * emit_remaining;
        emit_remaining = 1.0f;

        if (!((zNPC::zNPCCommon*)ent)->inSlowEnemyCollisionVolume) {
            xMat4x3 mat;
            mat.up = up;
            mat.at = dir;
            mat.right.cross(mat.up, mat.at);
            mat.right.safe_normalize(*xEntGetRightDirection(ent));

            if (xrand_RandomInt32() & 1) {
                mat.at.negate();
            }

            mat.pos = pos;
            mat.pos.y += xrand_RandomFloatRange(0.0f, 0.05f);

            if (supersize) {
                debrisEmitter.set_config(debrisConfigSupersize);
            } else {
                debrisEmitter.set_config(debrisConfig);
            }

            debrisEmitter.spawn(mat, -1.0f, 0);
        } else if (trailDecal.texture != NULL) {
            xMat4x3 mat;
            xVec3Copy(&mat.pos, &pos);
            mat.pos.AddScale(up, 0.5f * trailDecal.max_dist);
            mat.up = dir;
            mat.right = *xEntGetRightDirection(ent);
            mat.at.cross(mat.right, mat.up);

            F32 scale = supersize ? xrand_RandomFloatRange(3.0f, 3.74f) : xrand_RandomFloatRange(1.5f, 1.87f);
            trailDecal.size.assign(scale, scale);

            xDecal::emit(trailDecal, mat, k_XENT_COLLTYPE_STAT | k_XENT_COLLTYPE_DYN | k_XENT_COLLTYPE_ENV);
        }
    }

    emit_remaining -= dist / 0.7f;
}

void zFX::zNPCDrillMovementDust::EmitDebris(const xVec3& at, const xVec3& right, const xVec3& up_, const xVec3& from,
                                            const xVec3& to, F32& emit_remaining, U32 trail, bool supersize)
{
    xVec3 dir;
    dir.Sub(to, from);
    F32 dist = dir.length();

    if (!(dist > 0.7f * emit_remaining)) {
        emit_remaining -= dist / 0.7f;
        return;
    }

    dir.safe_normalize(at);

    xVec3 up = up_;
    up.AddScale(dir, -up.dot(dir));
    up.safe_normalize(up_);

    if (need_setup) {
        need_setup = false;
        scene_setup();
    }

    xVec3 pos = from;

    while (dist > 0.7f * emit_remaining) {
        pos.AddScale(dir, 0.7f * emit_remaining);
        dist -= 0.7f * emit_remaining;
        emit_remaining = 1.0f;

        if (!trail) {
            xMat4x3 mat;
            mat.up = up;
            mat.at = dir;
            mat.right.cross(mat.up, mat.at);
            mat.right.safe_normalize(right);

            if (xrand_RandomInt32() & 1) {
                mat.at.negate();
            }

            mat.pos = pos;
            mat.pos.y += xrand_RandomFloatRange(0.0f, 0.05f);

            if (supersize) {
                debrisEmitter.set_config(debrisConfigSupersize);
            } else {
                debrisEmitter.set_config(debrisConfig);
            }

            debrisEmitter.spawn(mat, -1.0f, 0);
        } else if (trailDecal.texture != NULL) {
            xMat4x3 mat;
            xVec3Copy(&mat.pos, &pos);
            mat.pos.AddScale(up, 0.5f * trailDecal.max_dist);
            mat.up = dir;
            mat.right = right;
            mat.at.cross(mat.right, mat.up);

            F32 scale = supersize ? xrand_RandomFloatRange(3.0f, 3.74f) : xrand_RandomFloatRange(1.5f, 1.87f);
            trailDecal.size.assign(scale, scale);

            xDecal::emit(trailDecal, mat, k_XENT_COLLTYPE_STAT | k_XENT_COLLTYPE_DYN | k_XENT_COLLTYPE_ENV);
        }
    }

    emit_remaining -= dist / 0.7f;
}

void zFX::zNPCDrillMovementDust::emit(const xVec3& position, const xVec3& normal, F32& emit_remaining,
                                      const xVec3& velocity, F32 dt, config* cfg, S32 type)
{
    if (type == -1) {
        _emit(position, normal, emit_remaining, velocity, dt, cfg, 0);
        _emit(position, normal, s_clods_emit_remaining, velocity, dt, &sNPCDrillerMovementClodsConfig, 1);
    } else {
        _emit(position, normal, emit_remaining, velocity, dt, cfg, (U8)type);
    }
}

void zFX::zNPCDrillMovementDust::_emit(const xVec3& position, const xVec3& normal, F32& emit_remaining,
                                       const xVec3& velocity, F32 dt, config* cfg, U32 type)
{
    if (need_setup) {
        need_setup = false;
        scene_setup();
    }

    S32 system_id;
    switch (type) {
    case 0:
        system_id = dust_system_id;
        break;
    case 1:
        system_id = clod_system_id;
        break;
    }

    F32 time_passed = (1.0f - emit_remaining) / cfg->emit_rate;
    emit_remaining += cfg->emit_rate * dt;

    S32 emit = emit_remaining;

    while (emit > 0) {
        particle* p;
        S32 count = xParticleBatchEmit(system_id, emit, (U8**)&p);
        if (count == 0) {
            emit_remaining = 0.0f;
            break;
        }

        particle* end = p + count;

        while (p != end) {
            if (p == end) {
                break;
            }

            p->life = xurand() * (cfg->max_life - cfg->min_life) + cfg->min_life - time_passed;
            p->size = xurand() * (cfg->max_size - cfg->min_size) + cfg->min_size;
            p->vel.assign(xurand() - 0.5f, xurand() - 0.5f, xurand() - 0.5f);
            p->vel.safe_normalize(g_O3);
            p->vel *= xurand() * (cfg->velocity_max - cfg->velocity_min) + cfg->velocity_min;
            p->vel.x *= cfg->velocity_dir_scale.x;
            p->vel.y *= cfg->velocity_dir_scale.y;
            p->vel.z *= cfg->velocity_dir_scale.z;
            p->vel += cfg->start_velocity;
            p->rot = PI * (xurand() - 0.5f);
            p->rot_delta = xurand() * (cfg->max_rot - cfg->min_rot) + cfg->min_rot;

            F32 dir_dot = p->vel.dot(normal);
            if (dir_dot < 0.0f) {
                p->vel.AddScale(normal, -2.0f * dir_dot);
            }

            p->texture_index = 4.0f * xurand();

            F32 vel_length = p->vel.length();
            p->vel.Scale(MAX(vel_length - cfg->slow * dt, 0.0f) / vel_length);
            p->vel.y += cfg->size_delta / 2.0f;
            p->vel.y -= cfg->gravity * dt;
            p->pos.AddScale(position, velocity, time_passed);
            p->pos.y += p->size / 2.0f;
            p->cfg = cfg;
            p->pos.AddScale(p->pos, p->vel, dt);

            dt -= 1.0f / cfg->emit_rate;
            time_passed += 1.0f / cfg->emit_rate;
            p++;
        }

        emit_remaining -= count;
        emit -= count;
    }
}

S32 zFX::zNPCDrillMovementDust::update(U8* mem, S32 count, ptank_pool& pool_, F32 dt, void*)
{
    ptank_pool__pos_color_size_rot_uv2& pool = (ptank_pool__pos_color_size_rot_uv2&)pool_;
    S32 cinematic_not_running = globals.cmgr == NULL ? TRUE : FALSE;

    particle* p = (particle*)mem;
    particle* end = p + count;

    while (p != end) {
        p->life -= dt;

        if (p->life < 0.0f || (cinematic_not_running & p->cfg->cinematic_only)) {
            *p = *--end;
            continue;
        }

        F32 vel_length = MAX(0.0001f, p->vel.length());
        p->vel.Scale(MAX(vel_length - p->cfg->slow * dt, 0.0f) / vel_length);
        p->vel.y -= p->cfg->gravity * dt;
        p->pos.AddScale(p->vel, dt);

        p->rot += p->rot_delta * dt;
        p->rot = xDangleClamp(p->rot);

        p->size += p->cfg->size_delta * dt;

        RwSphere worldsph;
        worldsph.center = p->pos.m_RwV3d;
        worldsph.radius = p->size;

        if (RwCameraFrustumTestSphere(xglobals->screen->icam, &worldsph) != rwSPHEREOUTSIDE) {
            pool.next();
            if (!pool.valid()) {
                end = p;
                break;
            }

            *pool.pos = p->pos;

            pool.color->b = p->cfg->b;
            pool.color->g = p->cfg->g;
            pool.color->r = p->cfg->r;

            if (p->texture_index > 1) {
                pool.uv->y = 0.5f;
            } else {
                pool.uv->y = 0.0f;
            }

            if (p->texture_index % 2 == 1) {
                pool.uv->x = 0.5f;
            } else {
                pool.uv->x = 0.0f;
            }

            pool.uv[1].assign(pool.uv->x + 0.5f, pool.uv->y + 0.5f);

            pool.SetRot(p->rot);

            F32 alpha;
            if (p->life < p->cfg->fade_start) {
                alpha = p->life / p->cfg->fade_start;
            } else {
                alpha = 1.0f;
            }

            pool.color->a = 255.0f * (alpha * p->cfg->glow);

            pool.size->assign(p->size, p->size);
        }

        p++;
    }

    return end - (particle*)mem;
}
