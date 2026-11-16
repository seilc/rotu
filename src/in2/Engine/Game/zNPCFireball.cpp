#include "xCommon.h"
#include "zNPCFireball.h"

#include "xBound.h"
#include "xCollide.h"
#include "xDecal.h"
#include "xEnt.h"
#include "xGlobals.h"
#include "xMath3.h"
#include "xModel.h"
#include "xScene.h"
#include "xstransvc.h"
#include "xutil.h"
#include "xVec3.h"
#include "xVec3Inlines.h"
#include "zBase.h"
#include "zBomb.h"
#include "zDestructible.h"
#include "zGlobals.h"
#include "zHashes.h"
#include "zLightWeight.h"
#include "zPlayer.h"
#include "zReactiveAnimation.h"
#include "zScene.h"
#include "zSparkEmitter.h"

#include <string.h>

#define MAX_FIREBALLS 40

namespace {

enum DecalType
{
    DECAL_TYPE_NONE,
    DECAL_TYPE_DAMAGEABLE,
    DECAL_TYPE_ENV,
    DECAL_TYPE_ENT
};

struct FireballParams
{
    U32 modelID;
    U32 textureID;
    F32 decalSize;
    F32 decalLife;
    F32 range;
    F32 speed;
    F32 spinSpeed;
    F32 damageRadius;
    S16 damage;
    RpAtomic* model;
    RwTexture* texture;
};

FireballParams fireballParams[] = {
    { HASH_proj_excav_bullet, HASH_fx_excav_pock, 0.5f, 5.0f, 5.0f, 20.0f, 6.0f * PI, 1.0f, 20 },
    { HASH_proj_excav_bullet, HASH_fx_excav_pock, 0.5f, 5.0f, 5.0f, 20.0f, 6.0f * PI, 1.0f, 30 }
};

const xDecal::curve_node decalCurve[] = {
    { 0.0f, { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f } },
    { 0.9f, { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f } },
    { 1.0f, { 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f } }
};

xResponseCurve decalResponseCurve;
zSparkEmitterAsset sparks;

void RayCast(xCollis& coll, const xEnt* ignore, const xVec3& origin, const xVec3& dir, F32 dist, U8 collFlags)
{
    xRay3 ray;
    ray.origin = origin;
    ray.dir = dir;
    ray.min_t = 0.0f;
    ray.max_t = dist;
    ray.flags = XRAY3_USE_MAX;

    coll.flags = 0;
    coll.dist = 1e38f;
    coll.optr = NULL;
    coll.mptr = NULL;
    xRayHitsSceneFlags(globals.sceneCur, &ray, &coll, k_XENT_COLLTYPE_NPC, collFlags);

    if (coll.optr == ignore) {
        F32 skip = coll.dist + 0.01f;
        xVec3 newOrigin;
        newOrigin.AddScale(origin, dir, skip);
        RayCast(coll, ignore, newOrigin, dir, dist - skip, collFlags);
        return;
    }

    if (coll.flags & k_HIT_IT) {
        ray.max_t = coll.dist;
    }

    if (collFlags & k_XENT_COLLTYPE_PC) {
        xCollis playerColl;
        playerColl.flags = 0;
        playerColl.dist = 1e38f;
        playerColl.optr = NULL;
        playerColl.mptr = NULL;

        for (S32 i = 0; i < xglobals->players.size(); i++) {
            const xSphere* spheres[5];
            S32 count = xglobals->players[i]->GetCollisionSpheres(spheres);
            for (S32 j = 0; j < count; j++) {
                xBound bound;
                bound.type = k_XBOUNDTYPE_SPHERE;
                bound.sph = *spheres[j];
                xQuickCullForBound(&bound.qcd, &bound);
                xRayHitsBound(&ray, &bound, &playerColl);
                if (playerColl.flags & k_HIT_IT) {
                    coll = playerColl;
                    coll.optr = xglobals->players[i];
                    return;
                }
            }
        }
    }
}

bool DamageEnt(xEnt* ent)
{
    return ent->baseType == eBaseTypePlayer || ent->baseType == eBaseTypeNPC || zReactiveIsReactive(*ent) ||
           zDestructibleGetFromEntity(ent) != NULL;
}

DecalType GetDecalType(xEnt* ent)
{
    if (ent == NULL) {
        return DECAL_TYPE_ENV;
    }
    return DamageEnt(ent) ? DECAL_TYPE_DAMAGEABLE : DECAL_TYPE_ENT;
}

class zNPCFireball
{
public:
    static void SceneSetup();
    void Setup();
    void Reset();
    void Start(xEnt* newOwner, const xVec3& pos, const xVec3& newDir, bool newStrong);
    void Update(F32 dt);
    void Explode(F32 dt);
    void Render();
    void Exit();
    void PostRender3d() const {}
    bool IsActive() { return time > 0.0f; }
    void PostRender2d() const {}

private:
    bool strong;
    xVec3 dir;
    xModelInstance* model;
    F32 time;
    xVec3 lastPos;
    F32 rayTimer;
    DecalType decalType;
    xEnt* owner;
};

zLightweightList<zNPCFireball> fireballList;

void zNPCFireball::SceneSetup()
{
    for (S32 i = 0; i < ARRAY_LENGTH(fireballParams); i++) {
        fireballParams[i].model = xModelFindAtomic(fireballParams[i].modelID, NULL);
        fireballParams[i].texture = (RwTexture*)xSTFindAsset(fireballParams[i].textureID, NULL);
    }

    decalResponseCurve.reset(sizeof(xDecal::animix) / sizeof(F32), decalCurve, ARRAY_LENGTH(decalCurve));
    sparks.SetDefaults();
    sparks.num_sparks = 5;
}

void zNPCFireball::Setup()
{
    model = NULL;
    strong = false;
    time = 0.0f;
}

void zNPCFireball::Reset()
{
    time = 0.0f;
}

void zNPCFireball::Start(xEnt* newOwner, const xVec3& pos, const xVec3& newDir, bool newStrong)
{
    owner = newOwner;

    if (model == NULL || newStrong != strong) {
        strong = newStrong;
        if (model) {
            xModelInstanceFree(model, true);
        }
        model = xModelInstanceAlloc(fireballParams[strong].model, NULL, 0, 0, NULL);
        if (model == NULL) {
            time = 0.0f;
            return;
        }
        model->modelID = fireballParams[strong].modelID;
    }

    dir = newDir;
    time = fireballParams[strong].range / fireballParams[strong].speed;

    xMat4x3* mat = (xMat4x3*)model->Mat;
    mat->at = dir;
    mat->right.cross(xVec3::m_UnitAxisY, dir);
    mat->up.cross(dir, mat->right);
    mat->pos = pos;

    if (owner) {
        xCollis coll;
        RayCast(coll, owner, pos, dir, time * fireballParams[strong].speed,
                k_XENT_COLLTYPE_STAT | k_XENT_COLLTYPE_ENV);
        if (coll.flags & k_HIT_IT) {
            time = coll.dist / fireballParams[strong].speed;
            decalType = GetDecalType((xEnt*)coll.optr);
        } else {
            decalType = DECAL_TYPE_NONE;
        }
    } else {
        decalType = DECAL_TYPE_NONE;
    }

    rayTimer = 0.0f;
    lastPos = pos;
}

void zNPCFireball::Update(F32 dt)
{
    if (dt > time) {
        dt = time;
    }

    time -= dt;
    xVec3AddScaled((xVec3*)&model->Mat->pos, &dir, dt * fireballParams[strong].speed);

    xMat3x3 rot;
    xMat3x3Rot(&rot, &dir, dt * fireballParams[strong].spinSpeed);
    xMat3x3Mul((xMat3x3*)model->Mat, (xMat3x3*)model->Mat, &rot);

    if (time <= 0.0f) {
        Explode(dt);
        return;
    }

    rayTimer += dt;
    if (rayTimer >= 0.1f && owner) {
        xCollis coll;
        RayCast(coll, owner, lastPos, dir, rayTimer * fireballParams[strong].speed,
                k_XENT_COLLTYPE_DYN | k_XENT_COLLTYPE_NPC | k_XENT_COLLTYPE_PC);
        if (coll.flags & k_HIT_IT) {
            xVec3AddScaled(*(xVec3*)&model->Mat->pos, lastPos, dir, coll.dist);
            decalType = GetDecalType((xEnt*)coll.optr);
            Explode(dt);
        } else {
            xVec3Copy(&lastPos, (xVec3*)&model->Mat->pos);
            rayTimer = 0.0f;
        }
    }
}

void zNPCFireball::Explode(F32 dt)
{
    time = 0.0f;

    FireballParams& params = fireballParams[strong];
    if (owner) {
        sphere_damage(owner, *(xVec3*)&model->Mat->pos, params.damageRadius, params.damage, 0.0f, 0.0f,
                      zHS_ENEMY_FLYBACK, zHT_GENERAL);
    }

    xVec3 sparkDir;
    if (decalType != DECAL_TYPE_NONE) {
        sparkDir = xVec3::m_Null;
    } else {
        sparkDir = dir;
    }
    zSparkEmitter::EmitSparks(sparks, *(xVec3*)&model->Mat->pos, dir, dt, -1, -1, ISND_INVALID_SND_GROUP_HANDLE);

    if (params.texture) {
        if (decalType == DECAL_TYPE_ENV || decalType == DECAL_TYPE_ENT) {
            xDecal::emit_context decalContext;
            memset(&decalContext, 0, sizeof(decalContext));
            decalContext.flags = xDecal::emit_context::FLAG_ANIM_COLOR | xDecal::emit_context::FLAG_ANIM_UV;
            decalContext.blend_mode = xDecal::BLEND_MODE_AVERAGE;
            decalContext.curve = &decalResponseCurve;
            decalContext.uv[0].assign(0.0f, 0.0f);
            decalContext.uv[1].assign(1.0f, 1.0f);
            decalContext.texture = params.texture;
            decalContext.life = params.decalLife;
            decalContext.max_dist = 0.2f;
            decalContext.size.assign(params.decalSize, params.decalSize);

            xMat4x3 mat;
            xVec3Copy(&mat.pos, (xVec3*)&model->Mat->pos);
            xVec3 at;
            at.Negate(dir);
            at.y = 0.0f;
            xMat3x3LookVec(&mat, &at);
            mat.pos.AddScale(mat.at, -0.1f);

            if (decalType == DECAL_TYPE_ENT) {
                xDecal::emit(decalContext, mat, k_XENT_COLLTYPE_STAT);
            } else {
                xDecal::emit(decalContext, mat, k_XENT_COLLTYPE_ENV);
            }
        }
    }
}

void zNPCFireball::Render()
{
    xModelRender(model);
}

void zNPCFireball::Exit()
{
    if (model) {
        xModelInstanceFree(model, true);
        model = NULL;
    }
}

} // namespace

void zNPCFireballSetup()
{
    fireballList.Init(MAX_FIREBALLS);
}

void zNPCFireballLaunch(xEnt* owner, const xVec3& pos, const xVec3& dir, bool strong)
{
    zNPCFireball* fireball = fireballList.GetFree();
    if (fireball) {
        fireball->Start(owner, pos, dir, strong);
    }
}
