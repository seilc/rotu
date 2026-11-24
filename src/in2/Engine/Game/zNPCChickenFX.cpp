#include "xCommon.h"
#include "zNPCChickenFX.h"

#include "xCam.h"
#include "xGlobals.h"
#include "xMath.h"
#include "xParticleBatch.h"
#include "xRand.h"
#include "xString.h"
#include "xstransvc.h"

S32 zNPCChickenFXFeather::systemID = -1;
bool zNPCChickenFXFeather::needSetup;
zNPCChickenFXFeather::config zNPCChickenFXFeather::defaultConfig;

static bool sFlip;

void zNPCChickenFXFeather::Setup()
{
    needSetup = true;
    defaultConfig.minLife = 2.0f;
    defaultConfig.maxLife = 1.5f;
    defaultConfig.fadeStart = 0.7f;
    defaultConfig.r = 255;
    defaultConfig.g = 255;
    defaultConfig.b = 255;
    defaultConfig.size = 0.2f;
}

void zNPCChickenFXFeather::SceneSetup()
{
    xParticleBatchSystem system;
    system.rs.texture = (RwTexture*)xSTFindAsset(xStrHash("chicken_feather"), NULL);
    system.context = NULL;
    system.update = Update;
    system.pool_type = PGT_POS_COLOR_SIZE_ROT_UV2;
    system.rs.flags = 0;
    system.rs.src_blend = rwBLENDSRCALPHA;
    system.rs.dst_blend = rwBLENDINVSRCALPHA;
    system.stride = sizeof(particle);
    systemID = xPARTICLEBATCHADDSYSTEM(system);
}

void zNPCChickenFXFeather::Emit(const xVec3& pos, S32 emit, F32, config* cfg)
{
    if (needSetup) {
        SceneSetup();
        needSetup = false;
    }

    emit = xParticleBatchRequest(systemID, emit, 0);
    while (emit > 0) {
        particle* p;
        S32 count = xParticleBatchEmit(systemID, emit, (U8**)&p);
        if (count == 0) {
            break;
        }

        particle* end = p + count;
        while (p != end) {
            p->pos = pos;
            p->pos.y += 0.1f * xurand() - 0.05f;
            p->life = cfg->minLife + xurand() * (cfg->maxLife - cfg->minLife);
            p->flip = sFlip ? FALSE : TRUE;
            p->fadeTime = p->life * (1.0f - cfg->fadeStart);
            p->size = cfg->size;
            p->vel.assign(xurand() - 0.8f, xurand() - 0.8f);
            p->rot = PI * (0.3f * xurand() - 0.15f);
            p->rotChange = !p->flip ? PI : -PI;
            p->cfg = cfg;
            sFlip = !sFlip;
            p++;
        }

        emit -= count;
    }
}

S32 zNPCChickenFXFeather::Update(U8* mem, S32 count, ptank_pool& pool_, F32 dt, void*)
{
    ptank_pool__pos_color_size_rot_uv2& pool = (ptank_pool__pos_color_size_rot_uv2&)pool_;
    particle* p = (particle*)mem;
    particle* end = p + count;

    while (p != end) {
        p->life -= dt;
        if (p->life < 0.0f) {
            end--;
            *p = *end;
            continue;
        }

        p->rot += p->rotChange * dt;
        p->rot = xDangleClamp(p->rot);

        bool reverse = false;
        if (!p->flip) {
            if (p->rot < 0.0f || p->rot > PI) {
                reverse = true;
            }
        } else {
            if (p->rot > 0.0f || p->rot < -PI) {
                reverse = true;
            }
        }

        if (reverse) {
            p->rotChange = -p->rotChange;
            p->vel.x = -p->vel.x;
            p->vel.y = -p->vel.y;
        }

        p->pos.x += p->vel.x * dt;
        p->pos.y += -0.3f * dt;
        p->pos.z += p->vel.y * dt;

        xIF_SPHERE_VISIBLE(p->pos.m_RwV3d, p->size) {
            pool.next();
            if (!pool.valid()) {
                end = p;
                break;
            }

            *pool.pos = p->pos;
            pool.color->b = p->cfg->b;
            pool.color->g = p->cfg->g;
            pool.color->r = p->cfg->r;
            pool.SetRot(p->rot);

            if (p->life <= p->fadeTime) {
                pool.color->a = 255.0f * (p->life / p->fadeTime);
            } else {
                pool.color->a = 255;
            }

            pool.size->assign(p->size, p->size);
            pool.uv[0].y = 0.0f;
            pool.uv[1].y = 1.0f;
            if (!p->flip) {
                pool.uv[0].x = 0.0f;
                pool.uv[1].x = 1.0f;
            } else {
                pool.uv[0].x = 1.0f;
                pool.uv[1].x = 0.0f;
            }
        }

        p++;
    }

    return end - (particle*)mem;
}
