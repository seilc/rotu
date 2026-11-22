#include "xCommon.h"
#include "zNPCProjectileExplodingGrub.h"

#include "xCollideSweptSphere.h"
#include "xEnt.h"
#include "xEnvAsset.h"
#include "xGlobals.h"
#include "xModel.h"
#include "xScene.h"
#include "xSndMgr.h"
#include "xString.h"
#include "xstransvc.h"
#include "xSurface.h"
#include "xutil.h"
#include "zBase.h"
#include "zEnv.h"
#include "zGlobals.h"
#include "zNPCCommon.h"
#include "zNPCProjectileGrub.h"
#include "zNPCProjectileGrubFX.h"
#include "zPlayer.h"
#include "zScene.h"
#include "zShrapnel.h"

namespace {

xTWEAKABLE F32 EGRUB_SPEED = 40.0f;
xTWEAKABLE F32 EGRUB_EXPLOSION_RADIUS = 2.5f;
xTWEAKABLE F32 EGRUB_TRAIL_OFFSET = -0.3f;
xTWEAKABLE F32 EGRUB_SHADOW_OFFSET = 0.3f;
xTWEAKABLE F32 EGRUB_DECAL_SIZE = 6.0f;
xTWEAKABLE F32 EGRUB_LIFETIME = 10.0f;
xTWEAKABLE F32 EGRUB_RADIUS = 0.24f;

const xDecal::curve_node decalCurve[] = {
    { 0.0f, { 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f } },
    { 0.025f, { 1.0f, 1.0f, 1.0f, 0.8f, 1.0f, 1.0f } },
    { 0.4f, { 1.0f, 1.0f, 1.0f, 0.8f, 1.0f, 1.0f } },
    { 0.8f, { 1.0f, 1.0f, 1.0f, 0.27f, 1.0f, 1.0f } },
    { 1.0f, { 1.0f, 1.0f, 1.0f, 0.0f, 0.7f, 0.7f } }
};

const U32 FLY_SOUND_ID = xStrHash("GRUBFLY");
const U32 LAUNCH_SOUND_ID = xStrHash("GRUBLAUNCH");
const U32 SPLAT_SOUND_ID = xStrHash("GRUBHIT");
const U32 EGRUB_BUG_MODEL_ID = xStrHash("proj_grub_UM");
const U32 EGRUB_BUG_SURFACE_ID = xStrHash("FX_GRUB");

zNPCProjectileGrubTrail::TrailConfig PuffConfig = {
    -HUGE, -0.2f, 0.2f, 0.8f, { 0.0f, 0.0f, 0.0f }, 20.0f, -2.0f, 2.0f, 0.5f, 1.0f, 0.6f, 1.0f, 0.5f, 0.0f, 0
};
zNPCProjectileGrubTrail::TrailConfig ShortTrailConfig = {
    -HUGE, -0.15f, 0.1f, 0.6f, { 0.0f, 0.0f, 0.0f }, 10.0f, -2.0f, 2.0f, 0.7f, 0.25f, 0.5f, 1.0f, 0.5f, 0.0f, 1
};

} // namespace

xSndGroupHandle zNPCProjectileExplodingGrub::launchSoundGroup = ISND_INVALID_SND_GROUP_HANDLE;
xSndGroupHandle zNPCProjectileExplodingGrub::flyingSoundGroup = ISND_INVALID_SND_GROUP_HANDLE;
xSndGroupHandle zNPCProjectileExplodingGrub::splatSoundGroup = ISND_INVALID_SND_GROUP_HANDLE;

xSurface* egrubScrollingSurf;

F32 zNPCProjectileExplodingGrub::GetRadius()
{
    return EGRUB_RADIUS;
}

void zNPCProjectileExplodingGrub::Freeze()
{
    xVec3 velocity;
    velocity.Scale(dir, speed);
}

void zNPCProjectileExplodingGrub::StaticSetup()
{
    flyingSoundGroup = xSndMgrGetSoundGroup(FLY_SOUND_ID);
    launchSoundGroup = xSndMgrGetSoundGroup(LAUNCH_SOUND_ID);
    splatSoundGroup = xSndMgrGetSoundGroup(SPLAT_SOUND_ID);
    egrubScrollingSurf = (xSurface*)xSceneResolvID(globals.sceneCur, EGRUB_BUG_SURFACE_ID);
    egrubScrollingSurf->type = 2;
}

void zNPCProjectileExplodingGrub::Reset()
{
    Destroy();
}

void zNPCProjectileExplodingGrub::Update(F32 dt)
{
    lifetime -= dt;
    if (lifetime <= 0.0f) {
        Destroy();
        return;
    }

    xVec3 prev_pos = position, velocity;
    velocity.Scale(dir, speed);
    xVec3 dpos;
    dpos.Scale(velocity, dt);
    Collide(dpos, dt);

    if (IsActive()) {
        position += dpos;

        xVec3 cur_pos = position;
        if (bugModel) {
            prev_pos.AddScale(((xMat4x3*)bugModel->Mat)->at, EGRUB_TRAIL_OFFSET);
            cur_pos.AddScale(((xMat4x3*)bugModel->Mat)->at, EGRUB_TRAIL_OFFSET);
        }

        xVec3 trail_pos;
        xVec3Copy(&trail_pos, &position);
        if (bugModel) {
            trail_pos.AddScale(((xMat4x3*)bugModel->Mat)->at, EGRUB_TRAIL_OFFSET);
        }

        if (lifetime > EGRUB_LIFETIME - 0.5f) {
            xVec3 vel0 = { 0.0f, 0.0f, 0.0f };
            zNPCProjectileGrubTrail::Emit(emit_remaining, startPos, vel0, dt, -1.0f, &PuffConfig);
        }

        xVec3 step;
        step = cur_pos;
        step.Sub(prev_pos);
        xVec3SMulByMacro(&step, 1.0f / 3.0f);
        for (S32 i = 0; i < 3; i++) {
            zNPCProjectileGrubTrail::Emit(emit_remaining, prev_pos, velocity, dt, 0.4f, &ShortTrailConfig);
            prev_pos.Add(step);
        }

        zNPCProjectileEGrubSparks::Emit(spark_emit_remaining, trail_pos, velocity, dt, NULL);

        if (flyingSound == XSNDMGR_INVALID_SND_HANDLE && flyingSoundGroup != ISND_INVALID_SND_GROUP_HANDLE) {
            flyingSound = xSndMgrPlay(flyingSoundGroup, xSnd_FOLLOW, &position, &velocity, NULL, NULL, NULL);
        }

        if (bugModel) {
            xMat4x3 shadowMat;
            xMat4x3Copy(&shadowMat, (xMat4x3*)bugModel->Mat);
            shadowMat.pos.AddScale(((xMat4x3*)bugModel->Mat)->at, EGRUB_SHADOW_OFFSET);
            xShadowSimple_Add(&simpShadow, &shadowMat, 0.5f, 1.0f);
        }
    }
}

void zNPCProjectileExplodingGrub::Render()
{
    if (bugModel) {
        xMat4x3* frame = (xMat4x3*)bugModel->Mat;
        xVec3Copy(&frame->pos, &position);

        xMat3x3 rotMat;
        xMat3x3RotZ(&rotMat, 2.0f * PI * globals.update_dt);
        xMat3x3Mul(frame, &rotMat, frame);
        xModelRender(bugModel);
    }
}

void zNPCProjectileExplodingGrub::Destroy()
{
    active = false;
    if (bugModel) {
        xModelInstanceFree(bugModel, true);
        bugModel = NULL;
    }

    if (flyingSound != XSNDMGR_INVALID_SND_HANDLE && xSndMgrIsLooping(flyingSound)) {
        xSndMgrStop(flyingSound);
    }
}

bool zNPCProjectileExplodingGrub::Setup(void* configData)
{
    GrubConfig* config = (GrubConfig*)configData;
    owner = config->owner;

    if (!bugModel) {
        RpAtomic* data = xModelFindAtomic(EGRUB_BUG_MODEL_ID, NULL);
        if (!data) {
            return false;
        }

        bugModel = xModelInstanceAlloc(data, NULL, 0, 0, NULL);
        if (bugModel) {
            bugModel->modelID = EGRUB_BUG_MODEL_ID;
        }
        if (!bugModel) {
            return false;
        }

        zEnv* zen = globals.sceneCur->zen;
        xEnvAsset* easset = zen->easset;
        xModelSetLightKit(bugModel, (xLightKit*)xSTFindAsset(easset->objectLightKit, NULL), 0.0f);
    }

    position = config->startPos;
    startPos = config->startPos;
    dir = config->dir;
    dir.safe_normalize(g_NZ3);
    lifetime = EGRUB_LIFETIME;
    speed = config->speed <= 0.0f ? EGRUB_SPEED : config->speed;

    if (bugModel) {
        xMat4x3* frame = (xMat4x3*)bugModel->Mat;
        xVec3 up = { 0.0f, 1.0f, 0.0f };
        xVec3Copy(&frame->pos, &position);
        xVec3Copy(&frame->at, &dir);
        frame->right.cross(up, dir);
        frame->up.cross(dir, frame->right);
        bugModel->Surf = egrubScrollingSurf;
    }

    memset(&decalContext, 0, sizeof(decalContext));
    decalResponseCurve.reset(sizeof(xDecal::animix) / sizeof(F32), decalCurve, ARRAY_LENGTH(decalCurve));
    decalContext.flags = xDecal::emit_context::FLAG_ANIM_COLOR | xDecal::emit_context::FLAG_ANIM_UV;
    decalContext.blend_mode = xDecal::BLEND_MODE_AVERAGE;
    decalContext.curve = &decalResponseCurve;
    decalContext.uv[0].assign(0.0f, 0.0f);
    decalContext.uv[1].assign(1.0f, 1.0f);
    decalContext.texture = (RwTexture*)xSTFindAsset(xStrHash("fx_grub_splat_UM"), NULL);
    decalContext.life = 5.0f;
    decalContext.max_dist = 3.0f;
    decalContext.size.assign(EGRUB_DECAL_SIZE, EGRUB_DECAL_SIZE);

    xShadowSimple_CacheInit(&simpShadow, 0x50);
    damage = config->damage;
    emit_remaining = 0.0f;
    spark_emit_remaining = 0.0f;
    arc_emit_remaining = 0.0f;
    launchGoo = (zShrapnelAsset*)xSTFindAsset(xStrHash("egrub_launch_goo"), NULL);
    hitGoo = (zShrapnelAsset*)xSTFindAsset(xStrHash("egrub_hit_goo"), NULL);
    flyingSound = XSNDMGR_INVALID_SND_HANDLE;
    xSndMgrPlay(launchSoundGroup, 0, NULL, NULL, NULL, NULL, NULL);
    active = true;
    return true;
}

void zNPCProjectileExplodingGrub::Collide(const xVec3& dpos, F32)
{
    xVec3 velocity;
    velocity.Scale(dir, speed);
    if (xfeq0(velocity.length2())) return;

    xSweptSphere sws;
    memset(&sws, 0, sizeof(sws));
    xVec3 start = position, end;
    end.Add(position, dpos);
    xSweptSpherePrepare(&sws, &start, &end, GetRadius(), 0);

    if (xSweptSphereToStatDyn(&sws, xglobals->sceneCur, owner, k_XENT_COLLTYPE_PC, 0) ||
        xSweptSphereToEnv(&sws, xglobals->sceneCur->env)) {
        xSweptSphereGetResults(&sws);
        Explode(true, NULL);

        xMat4x3 mat;
        xVec3Copy(&mat.pos, &sws.worldContact);
        mat.pos.AddScale(sws.worldNormal, 0.5f * decalContext.max_dist);
        xVec3 at;
        xVec3Copy(&at, &sws.worldNormal);
        xMat3x3LookVec(&mat, &at);
        xDecal::emit(decalContext, mat, k_XENT_COLLTYPE_STAT | k_XENT_COLLTYPE_ENV);
        return;
    }

    if (xSweptSphereToNPC(&sws, xglobals->sceneCur, owner, k_XENT_COLLTYPE_PC, 0)) {
        xSweptSphereGetResults(&sws);
        if (((zNPC::zNPCCommon*)sws.optr)->GetNPCType() == 9) {
            Explode(true, (xEnt*)sws.optr);
        } else {
            Explode(false, (xEnt*)sws.optr);
        }
        return;
    }

    for (S32 i = 0; i < CURPLAYERS.size(); i++) {
        const xSphere* extra_spheres[5];
        S32 num_spheres = CURPLAYERS[i]->GetCollisionSpheres(extra_spheres);
        for (S32 j = 0; j < num_spheres; j++) {
            if (xSweptSphereToSphere(&sws, extra_spheres[j])) {
                Explode(true, CURPLAYERS[i]);
                return;
            }
        }
    }
}

void zNPCProjectileExplodingGrub::Explode(bool, const xEnt* ent)
{
    if (ent && ent->baseType == eBaseTypePlayer)
        zNPCProjectileGrubArc::Emit(arc_emit_remaining, &zNPCProjectileGrubArcPlayerConfig, ent->model, NULL, -1.0f, -1);
    else if (ent && ent->baseType == eBaseTypeNPC && !xModelIsPreinstanced(ent->model))
        zNPCProjectileGrubArc::Emit(arc_emit_remaining, &zNPCProjectileGrubArcNPCConfig, ent->model, NULL, -1.0f, -1);

    xVec3 vel = { 0.0f, 0.0f, 0.0f };
    xMat4x3 lookFrame;
    xMat4x3Identity(&lookFrame);

    xMat4x3* frame;
    if (bugModel) {
        frame = (xMat4x3*)bugModel->Mat;
    } else if (ent) {
        frame = (xMat4x3*)ent->model->Mat;
    } else {
        xMat3x3LookVec3(lookFrame, dir);
        frame = &lookFrame;
        lookFrame.pos = GetPosition();
    }

    xMat4x3 fxFrame;
    xMat4x3Copy(&fxFrame, frame);
    fxFrame.pos.AddScale(fxFrame.at, 0.5f);
    zNPCProjectileGrubBigHitExplosionEmit(fxFrame, EGRUB_EXPLOSION_RADIUS, owner, damage);
    zNPCProjectileEGrubSparks::Emit(spark_emit_remaining, fxFrame.pos, vel, 1.0f, &burstESparkConfig);

    if (hitGoo) {
        hitGoo->initCB(hitGoo, bugModel, NULL, &vel, NULL);
    }

    if (flyingSound != XSNDMGR_INVALID_SND_HANDLE && xSndMgrIsLooping(flyingSound)) {
        xSndMgrStop(flyingSound);
    }

    xSndMgrPlay(splatSoundGroup, 0, NULL, NULL, NULL, NULL, NULL);
    active = false;
}

bool zNPCProjectileExplodingGrub::LaunchFromEnt(xEnt* ent, const xVec3* localOffset, F32 speed, xEnt* refObj)
{
    GrubConfig config;
    memset(&config, 0, sizeof(config));
    config.owner = ent;
    config.damage = 10;
    config.speed = speed;

    xMat4x3* mat;
    if (refObj) {
        mat = &refObj->frame->mat;
    } else {
        mat = &ent->frame->mat;
    }

    xVec3 offset;
    xMat3x3RMulVec(&offset, mat, localOffset);
    xVec3Add(&config.startPos, &mat->pos, &offset);
    xVec3Copy(&config.dir, &mat->at);
    return Setup(&config);
}
