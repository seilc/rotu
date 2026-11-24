#include "xCommon.h"
#include "zTurretSmoke.h"

#include "xCam.h"
#include "xGlobals.h"
#include "xParticleBatch.h"
#include "xRand.h"
#include "xSpacePartitionTree.h"
#include "xString.h"
#include "xstransvc.h"

#define SMOKE_PARTICLE_LIFE 0.8f

RwTexture* zTurretSmokeSystem::texture;

namespace
{
    S32 systemID;
    const xColor turretSmokeColor = { 0xFF, 0xFF, 0xFF, 0x28 };
} // namespace

void zTurretSmokeSystem::Setup()
{
    texture = (RwTexture*)xSTFindAsset(xStrHash("ExplosionFlash_Smoke_tan_64"), NULL);
    if (!texture) {
        systemID = -1;
        return;
    }

    xParticleBatchSystem system;
    system.context = NULL;
    system.update = Update;
    system.order_group = 'HOBO';
    system.order_index = 11;
    system.pool_type = PGT_POS_COLOR_SIZE_ROT;
    system.rs.texture = texture;
    system.rs.src_blend = rwBLENDSRCALPHA;
    system.rs.dst_blend = rwBLENDINVSRCALPHA;
    system.rs.flags = 0;
    system.stride = sizeof(TurretSmokeParticle);
    systemID = xPARTICLEBATCHADDSYSTEM(system);
}

zTurretSmokeSystem::zTurretSmokeSystem(const xVec3& pos, const xVec3& dir) : zLightweight<zTurretSmokeSystem, 10>(true)
{
    ResetOrientation(pos, dir);
}

void zTurretSmokeSystem::Stop()
{
    SetUsed(false);
}

zTurretSmokeSystem::zTurretSmokeSystem() : zLightweight<zTurretSmokeSystem, 10>(false)
{
}

void zTurretSmokeSystem::ResetOrientation(const xVec3& pos, const xVec3& dir)
{
    this->pos = pos;
    this->dir = dir;
    emitAccum = 0.0f;
    time = 0.1f;
    emitRemaining = 15.0f;
}

void zTurretSmokeSystem::Update(F32 dt)
{
    if (emitRemaining < 0.0f) {
        SetUsed(false);
        return;
    }

    emitRemaining -= dt;
    if (!texture) {
        return;
    }

    F32 timePassed = (1.0f - emitAccum) / 200.0f;
    emitAccum += xParticleBatchRequest(systemID, 200.0f * dt, 0);
    S32 emit = emitAccum;
    emitAccum -= emit;
    while (emit > 0) {
        TurretSmokeParticle* p;
        S32 count = xParticleBatchEmit(systemID, emit, (U8**)&p);
        if (count == 0) {
            emitAccum = 0.0f;
            return;
        }

        TurretSmokeParticle* end = p + count;
        while (p != end) {
            p->color = turretSmokeColor;
            p->size = 0.9f * xurand();
            p->vel.x = 0.2f * (xurand() - 0.5f);
            p->vel.y = 0.2f * (xurand() - 0.5f);
            p->vel.z = 0.2f * (xurand() - 0.5f);
            p->vel.AddScale(dir, 2.0f);
            p->vel.y = MAX(1.5f, p->vel.y);
            p->rot = 2.0f * PI * xurand() - PI;
            p->life = SMOKE_PARTICLE_LIFE - timePassed;
            p->pos = pos;
            p->pos.AddScale(p->pos, p->vel, timePassed);
            p->pos.y -= 0.5f * timePassed * timePassed;
            timePassed += 0.005f;
            p++;
        }

        emit -= count;
    }
}

S32 zTurretSmokeSystem::Update(U8* mem, S32 count, ptank_pool& pool_, F32 dt, void* context)
{
    zTurretSmokeSystem* system = (zTurretSmokeSystem*)context;
    ptank_pool__pos_color_size_rot& pool = (ptank_pool__pos_color_size_rot&)pool_;
    TurretSmokeParticle* p = (TurretSmokeParticle*)mem;
    TurretSmokeParticle* end = p + count;

    while (p != end) {
        p->vel.y -= dt;
        p->pos.AddScale(p->vel, dt);
        p->life -= dt;
        if (p->life < 0.0f) {
            end--;
            *p = *end;
            continue;
        }

        xIF_SPHERE_VISIBLE(p->pos.m_RwV3d, 1.0f) {
            pool.next();
            if (!pool.valid()) {
                break;
            }

            if (p->life < SMOKE_PARTICLE_LIFE * 0.2f) {
                p->color.a = 40.0f * (p->life / (SMOKE_PARTICLE_LIFE * 0.2f));
            } else {
                p->color.a = 40;
            }

            *pool.pos = p->pos;
            pool.SetRot(p->rot);
            *pool.color = p->color;
            pool.size->assign(p->size, p->size);
        }

        p++;
    }

    return end - (TurretSmokeParticle*)mem;
}
