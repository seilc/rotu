#include "xCommon.h"
#include "zFXFrozoneDodge.h"

#include "xAnim.h"
#include "xCam.h"
#include "xCollide.h"
#include "xGlobals.h"
#include "xParticleBatch.h"
#include "xPtankPool.h"
#include "xRand.h"
#include "xScene.h"
#include "xSpacePartitionTree.h"
#include "xstransvc.h"
#include "xutil.h"
#include "zCommonPlayer.h"
#include "zFrostEffect.h"
#include "zFrozoneBlast.h"
#include "zFrozonePlayerActions.h"
#include "zGlobals.h"
#include "zHashes.h"
#include "zScene.h"

namespace {

const zFrostIcicle::config dodgeIcicleConfig = { 0.5f, 3.0f, 0.5f, 0.25f, 0.1f, 1.5f };

} // namespace

void zFXFrozoneDodge::Setup()
{
    static const xDecal::curve_node decalCurve[] = {
        { 0.0f, { 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f } },
        { 0.025f, { 1.0f, 1.0f, 1.0f, 0.8f, 1.0f, 1.0f } },
        { 0.4f, { 1.0f, 1.0f, 1.0f, 0.8f, 1.0f, 1.0f } },
        { 0.8f, { 1.0f, 1.0f, 1.0f, 0.27f, 1.0f, 1.0f } },
        { 1.0f, { 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f } }
    };

    decalResponseCurve.reset(sizeof(xDecal::animix) / sizeof(F32), decalCurve, ARRAY_LENGTH(decalCurve));

    decalContext.owner = 'FZDG';
    decalContext.flags = xDecal::emit_context::FLAG_ANIM_COLOR | xDecal::emit_context::FLAG_ANIM_UV;
    decalContext.priority = 0;
    decalContext.blend_mode = xDecal::BLEND_MODE_AVERAGE;
    decalContext.curve = &decalResponseCurve;
    decalContext.texture = (RwTexture*)xSTFindAsset(HASH_ice_crystals_slide, NULL);
    decalContext.emitAtomicCB = NULL;
    decalContext.emitVertListCB = NULL;
    decalContext.destAtomicCB = NULL;
    decalContext.life = 3.0f;
    decalContext.max_dist = 3.0f;
    decalContext.context = (void*)&dodgeIcicleConfig;
    finished = false;

    zFXIceSpray::Init();
}

void zFXFrozoneDodge::Begin()
{
    decalTimer = 0.05f;
    decalCount = 0;
    firstUpdate = true;
    blastPending = true;
    finished = false;
    decalContext.emitAtomicCB = NULL;
    decalContext.emitVertListCB = NULL;
}

void zFXFrozoneDodge::End()
{
}

void zFXFrozoneDodge::Update(F32 dt, xEntFrame* frame, zFrozonePlayerDodge* dodge)
{
    if (firstUpdate) {
        firstUpdate = false;
        return;
    }

    xAnimSingle* single = dodge->player->model->Anim->Single;
    F32 duration = single->State->Data->Duration * single->CurrentSpeed;
    F32 remaining = duration - single->Time;

    xRay3 ray;
    xMat4x3 mat;
    mat.at.assign(0.0f, -1.0f, 0.0f);

    switch (dodge->dodgeDir) {
    case DODGE_BACK:
        ((zCommonPlayer*)dodge->player)->GetBonePos(&ray.origin, 26, &xVec3::m_Null, (xMat4x3*)dodge->player->model->Mat);
        mat.up.Negate(frame->mat.at);
        mat.right.Negate(frame->mat.right);
        break;
    case DODGE_FRONT:
        ((zCommonPlayer*)dodge->player)->GetBonePos(&ray.origin, 26, &xVec3::m_Null, (xMat4x3*)dodge->player->model->Mat);
        mat.up = frame->mat.at;
        mat.right = frame->mat.right;
        break;
    case DODGE_LEFT:
        ((zCommonPlayer*)dodge->player)->GetBonePos(&ray.origin, 26, &xVec3::m_Null, (xMat4x3*)dodge->player->model->Mat);
        mat.up = frame->mat.right;
        mat.right.Negate(frame->mat.at);
        break;
    case DODGE_RIGHT:
        ((zCommonPlayer*)dodge->player)->GetBonePos(&ray.origin, 38, &xVec3::m_Null, (xMat4x3*)dodge->player->model->Mat);
        mat.up.Negate(frame->mat.right);
        mat.right = frame->mat.at;
        break;
    }

    const xVec3& pos = frame->mat.pos;
    ray.dir.AddScale(pos, mat.up, 3.5f);
    ray.dir.Sub(ray.origin);
    ray.max_t = 1.0f;
    ray.min_t = 0.0f;
    ray.flags = XRAY3_USE_MIN | XRAY3_USE_MAX;

    xCollis coll;
    coll.flags = 0;
    coll.dist = 1e38f;
    coll.optr = NULL;
    coll.mptr = NULL;
    xRayHitsSceneFlags(globals.sceneCur, &ray, &coll, k_XENT_COLLTYPE_PC,
                       k_XENT_COLLTYPE_STAT | k_XENT_COLLTYPE_NPC | k_XENT_COLLTYPE_ENV);
    if (coll.flags & k_HIT_IT) {
        ray.max_t = coll.dist;
    }

    if (blastPending) {
        blastPending = false;
        zFrozoneBlast::SingleBlast(&ray, 30.0f, true);
    }

    mat.pos.AddScale(pos, mat.up, 1.0f);
    mat.pos.y += 0.5f;

    decalTimer -= dt;
    if (decalTimer < 0.0f && remaining > 0.5f) {
        decalTimer = 0.05f;
        decalContext.size.assign(2.0f, 3.0f);

        F32 u0 = (decalCount & 1) ? 0.5f : 0.0f;
        F32 u1;
        if (decalCount & 2) {
            u1 = u0;
            u0 = 0.5f + u0;
        } else {
            u1 = 0.5f + u0;
        }

        decalContext.uv[0].assign(u0, 0.0f);
        decalContext.uv[1].assign(u1, 1.0f);
        decalCount++;
        xDecal::emit(decalContext, mat, k_XENT_COLLTYPE_STAT | k_XENT_COLLTYPE_DYN | k_XENT_COLLTYPE_ENV);
    }

    if (remaining < 0.7f && remaining > 0.5f) {
        zFXIceSpray::Emit(pos, mat.up, dt, 0.0f);
    }
}

namespace {

S32 iceSpraySystemID;
F32 iceSprayTimeToEmit;

struct sprayParticle
{
    xVec3 pos;
    F32 life;
    xVec3 vel;
    U32 unk1C;
};

S32 updateSprayParticles(U8* mem, S32 count, ptank_pool& apool, F32 dt, void*)
{
    FPTANK_BEGIN(pos_color_size, apool);
    sprayParticle* p = (sprayParticle*)mem;
    sprayParticle* end = p + count;

    while (p != end) {
        p->life -= dt;
        if (p->life < 0.0f) {
            end--;
            *p = *end;
            continue;
        }

        p->vel.y -= 25.0f * dt;
        p->vel.Scale(0.95f);
        p->pos.AddScale(p->vel, dt);

        RwSphere worldsph;
        worldsph.center = p->pos.m_RwV3d;
        worldsph.radius = 0.2f;
        if (RwCameraFrustumTestSphere(xglobals->screen->icam, &worldsph)) {
            FPTANK_NEXT(pos_color_size);

            *pool_pos = p->pos;
            pool_size->assign(0.2f, 0.2f);
            U32 alpha = 255.0f * p->life;
            *pool_color = g_WHITE;
            pool_color->a = alpha;
        }

        p++;
    }

    FPTANK_END(pos_color_size);
    return p - (sprayParticle*)mem;
}

} // namespace

void zFXIceSpray::Init()
{
    xParticleBatchSystem system;
    system.pool_type = PGT_POS_COLOR_SIZE;
    system.rs.texture = (RwTexture*)xSTFindAsset(HASH_fx_nuke_fro_flake, NULL);
    system.rs.src_blend = rwBLENDSRCALPHA;
    system.rs.dst_blend = rwBLENDINVSRCALPHA;
    system.rs.flags = ptank_pool::FLAG_ORDERED;
    system.order_group = 'HOBO';
    system.order_index = 7;
    system.stride = sizeof(sprayParticle);
    system.update = updateSprayParticles;
    system.context = NULL;
    iceSpraySystemID = xPARTICLEBATCHADDSYSTEM(system);
}

void zFXIceSpray::Emit(const xVec3& pos, const xVec3& dir, F32 dt, F32 rate)
{
    if (!rate) {
        rate = 200.0f;
    }

    iceSprayTimeToEmit += rate * dt;
    S32 emit = (S32)iceSprayTimeToEmit;
    if (emit <= 0) {
        return;
    }

    iceSprayTimeToEmit -= emit;
    while (emit != 0) {
        U8* mem;
        S32 have = xParticleBatchEmit(iceSpraySystemID, emit, &mem);
        if (have == 0) {
            break;
        }

        emit -= have;
        sprayParticle* p = (sprayParticle*)mem;
        sprayParticle* end = p + have;
        while (p != end) {
            p->pos = pos;
            p->pos.x += 0.3f * xrand_RandomBipolarUnitFloat();
            p->pos.z += 0.3f * xrand_RandomBipolarUnitFloat();
            p->pos.AddScale(dir, 0.5f);
            p->vel.Scale(dir, 10.0f);
            p->vel.x += 5.0f * xrand_RandomBipolarUnitFloat();
            p->vel.z += 5.0f * xrand_RandomBipolarUnitFloat();
            p->vel.y += 5.0f * xrand_RandomFloat() + 5.0f;
            p->life = 1.0f;
            p++;
        }
    }
}
