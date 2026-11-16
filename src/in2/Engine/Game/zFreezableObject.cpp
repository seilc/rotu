#include "xCommon.h"
#include "zFreezableObject.h"

#include "iModel.h"
#include "xCollide.h"
#include "xEvent.h"
#include "xGlobals.h"
#include "xMath3.h"
#include "xModel.h"
#include "xRand.h"
#include "xRumbleEmitter.h"
#include "xstransvc.h"
#include "xVec3.h"
#include "zCam.h"
#include "zCombat.h"
#include "zEnt.h"
#include "zEvent.h"
#include "zHashes.h"
#include "zOneLinerSystem.h"
#include "zPlayer.h"
#include "zScene.h"
#include "zShrapnel.h"
#include "zSparkEmitter.h"

#include <string.h>

namespace {

zSparkEmitterAsset sSparkAsset;
xSndGroupHandle sSndRollerRun = ISND_INVALID_SND_GROUP_HANDLE;
xSndGroupHandle sSndIceShatter = ISND_INVALID_SND_GROUP_HANDLE;
zShrapnelAsset* sIceShrap;
F32 sLastDT;

} // namespace

F32 zFreezableObject::mTimeToFreezeMultiplier = 1.0f;

void zFreezableObject::Freezing()
{
    if (mTimeToFreeze >= mAsset->timeToFreeze) {
        zOneLinerSystemPlay(xglobals->players.GetFrozone(), zOLT_FreezeObject);
    }

    if (mCanFreeze) {
        mFreezing = true;
    }
}

void zFreezableObject::SetFreezeLevel(zCommonPlayerUpgradeLevel level)
{
    switch (level) {
    case ezCommonPlayerUpgradeLevel_Level2:
        mTimeToFreezeMultiplier = 1.6666666f;
        break;
    case ezCommonPlayerUpgradeLevel_Level1:
        mTimeToFreezeMultiplier = 1.25f;
        break;
    case ezCommonPlayerUpgradeLevel_Normal:
        mTimeToFreezeMultiplier = 1.0f;
        break;
    }
}

void zFreezableObject::InitDynamic(xBase& data, xDynAsset& asset, size_t)
{
    ((zFreezableObject&)data).Init((zFreezableObjectAsset*)&asset);
}

void zFreezableObject::Init(zFreezableObjectAsset* asset)
{
    mAsset = asset;

    this->asset = (xEntAsset*)xMEMALLOC(sizeof(xEntAsset), 0);
    memset(this->asset, 0, sizeof(xEntAsset));
    *(xBaseAsset*)this->asset = *asset;
    this->asset->id = asset->id;
    this->asset->baseType = asset->baseType;
    this->asset->baseFlags = asset->baseFlags | k_XBASE_IS_ENTITY;
    this->asset->flags = asset->flags & k_ZFREEZABLEOBJECT_START_ON;
    this->asset->pos = asset->pos;
    this->asset->scale = xVec3::m_Ones;
    this->asset->redMult = 1.0f;
    this->asset->greenMult = 1.0f;
    this->asset->blueMult = 1.0f;
    this->asset->seeThru = 1.0f;
    this->asset->modelInfoID = asset->modelID;
    linkCount = this->asset->linkCount;

    xEntInit(this, this->asset);

    if (this->asset->linkCount) {
        link = (xLinkAsset*)(asset + 1);
    } else {
        link = NULL;
    }

    zEntParseModelInfo(this, mAsset->modelID);
    xEntSetup(this);

    collType = k_XENT_COLLTYPE_DYN;
    collLev = k_XENT_COLLLEV_MODEL;
    moreFlags |= k_MORE_FLAGS_HITTABLE;
    bound.type = k_XBOUNDTYPE_BOX;
    bound.box.center = asset->pos;
    bound.box.center.y += 1.25f;
    bound.box.box.upper.Add(bound.box.center, 1.5f, 2.5f, 1.5f);
    bound.box.box.lower.Add(bound.box.center, -1.5f, -2.5f, -1.5f);

    xEntInitForType(this);

    update = Update;
    render = Render;
    eventFunc = EventCB;
}

void zFreezableObject::SceneEnter()
{
    sIceShrap = (zShrapnelAsset*)xSTFindAsset(HASH_ice_shatter, NULL);
    sSndRollerRun = xSndMgrGetSoundGroup(HASH_FREEZABLE_OBJ_RUN_SGRP);
    sSndIceShatter = xSndMgrGetSoundGroup(HASH_ICE_SHATTER_SGRP);

    sSparkAsset.SetDefaults();
    sSparkAsset.lifetime_min = 0.5f;
    sSparkAsset.lifetime_rand = 0.3f;
}

void zFreezableObject::Setup()
{
    RpAtomic* iceAtomic = (RpAtomic*)xSTFindAsset(HASH_fx_tower_ice, NULL);
    mIceModel = iceAtomic ? xModelInstanceAlloc(iceAtomic, NULL, 0, 0, NULL) : NULL;
    // BUG: writes through mIceModel before the NULL check
    mIceModel->modelID = HASH_fx_tower_ice;

    if (mAsset->shrapnelID) {
        mShrapnel = (zShrapnelAsset*)xSTFindAsset(mAsset->shrapnelID, NULL);
    } else {
        mShrapnel = NULL;
    }

    if (mAsset->killedTargetID) {
        mKilledTarget = zSceneFindObject(mAsset->killedTargetID);
    } else {
        mKilledTarget = NULL;
    }

    if (mAsset->driverID) {
        driver = (xEnt*)zSceneFindObject(mAsset->driverID);
    }

    mRumble = new (eMemStaticType_0, eMemMgrTag_FrozenObject) xRumble::FreezableObjectEffect();

    Reset();
}

void zFreezableObject::Reset()
{
    xEntReset(this);
    mHealth = 5.0f;

    if (mAsset->flags & k_ZFREEZABLEOBJECT_START_ON) {
        On();
    } else {
        Off();
    }

    if (mRumble) {
        mRumble->Init(HASH_FREEZABLE_OBJECT_RUMBLE_EMIT);
    }
}

void zFreezableObject::On()
{
    if (mHealth <= 0.0f) {
        return;
    }

    mCanFreeze = true;
    mTimeToFreeze = mAsset->timeToFreeze;
    mIceScaleMin = mAsset->iceScaleMin;
    mShatterTimer = 2.5f;
    mThawing = false;
    collType = k_XENT_COLLTYPE_DYN;
    chkby = k_XENT_COLLTYPE_NPC | k_XENT_COLLTYPE_PC;
    mRotation = 0.0f;
    mSparkTimer = 0.0f;
    mSparkDelay = 0.0f;
    mRollSound = xSndMgrPlay(sSndRollerRun, 0, NULL, NULL, this, NULL, NULL);

    if (!mActive) {
        xEntShow(this);
        mActive = true;
    }
}

void zFreezableObject::Off()
{
    mActive = false;
    xEntHide(this);
    collType = k_XENT_COLLTYPE_NONE;
    chkby = k_XENT_COLLTYPE_NONE;
    xSndMgrStop(mRollSound);
}

void zFreezableObject::Update(xEnt* ent, xScene* sc, F32 dt)
{
    sLastDT = dt;
    xEntUpdate(ent, sc, dt);
    ((zFreezableObject*)ent)->Update(dt);
}

void zFreezableObject::Update(F32 dt)
{
    if (!mActive || xVec3Dist2(&zCamGetMatrix().pos, (xVec3*)&model->Mat->pos) > 10000.0f) {
        return;
    }

    if (driver) {
        bound.box.center = *(xVec3*)&model->Mat->pos;
        bound.box.center.y += 1.25f;
        const xVec3& center = bound.box.center;
        bound.box.box.upper.Add(center, 1.5f, 2.5f, 1.5f);
        bound.box.box.lower.Add(center, -1.5f, -2.5f, -1.5f);
    }

    if (mFreezing && !mThawing) {
        mTimeToFreeze -= dt * mTimeToFreezeMultiplier;
        if (mTimeToFreeze < 0.0f) {
            mTimeToFreeze = 0.0f;
            zOneLinerSystemPlay(xglobals->players.GetFrozone(), zOLT_ShatterObject);
            Explode();
            return;
        }

        mShatterTimer = 2.5f;
        xSndMgrSetPitch(mRollSound, -18.0f * (1.0f - mTimeToFreeze / mAsset->timeToFreeze));
        mFreezing = false;
    } else if (mThawing || mTimeToFreeze != mAsset->timeToFreeze) {
        if (!mThawing) {
            mShatterTimer -= dt;
            if (mShatterTimer <= 0.0f) {
                EmitIceShrapnel(true);
                mThawing = true;
            }
        } else {
            mTimeToFreeze += 6.0f * dt;
            if (mTimeToFreeze >= mAsset->timeToFreeze) {
                mTimeToFreeze = mAsset->timeToFreeze;
                xSndMgrSetPitch(mRollSound, 0.0f);
                mThawing = false;
            } else {
                xSndMgrSetPitch(mRollSound, -18.0f * (1.0f - mTimeToFreeze / mAsset->timeToFreeze));
            }
        }
    }

    mRotation += 10.0f * (dt * (mTimeToFreeze / mAsset->timeToFreeze));
    while (mRotation >= PI) {
        mRotation -= 2.0f * PI;
    }
    while (mRotation < -PI) {
        mRotation += 2.0f * PI;
    }

    xMat3x3RotY((xMat3x3*)model->Mat, mRotation);

    if (mTimeToFreeze < mAsset->timeToFreeze && !mThawing) {
        EmitFreezeSparks(dt);

        if (mIceModel) {
            *mIceModel->Mat = *model->Mat;
            F32 scale = 1.5f * (1.0f - (1.0f - mIceScaleMin) * (mTimeToFreeze / mAsset->timeToFreeze));
            mIceModel->Scale.assign(scale, 1.0f, scale);
        }
    }

    for (S32 i = 0; i < xglobals->players.size(); i++) {
        const xSphere* spheres[5];
        S32 count = xglobals->players[i]->GetCollisionSpheres(spheres);
        xVec3* pos = (xVec3*)&model->Mat->pos;

        for (S32 j = 0; j < count; j++) {
            if (xSphereHitsVCylinder(spheres[j]->center, spheres[j]->r, *pos, 1.5f, 2.5f)) {
                xVec3 knockback;
                knockback.Sub(xglobals->players[i]->frame->mat.pos, *pos);
                knockback.y = 0.0f;
                knockback.safe_normalize(xglobals->players[i]->frame->mat.at);
                knockback.Scale(10.0f);
                knockback.y = 7.0f;
                xglobals->players[i]->Damage(zCombatDamageInfo(this, 5, zHS_KNOCKBACK, zHT_GENERAL, knockback,
                                                               xVec3::m_Null, xVec3::m_Null, 0));
                break;
            }
        }
    }
}

void zFreezableObject::EmitFreezeSparks(F32 dt)
{
    F32 ratio = mTimeToFreeze / mAsset->timeToFreeze;
    mSparkTimer += dt;
    if (mSparkTimer < ratio * mSparkDelay) {
        return;
    }

    mSparkTimer = 0.0f;
    EmitSparksNow(1, dt);
}

void zFreezableObject::EmitSparksNow(S32 count, F32 dt)
{
    F32 ratio = mTimeToFreeze / mAsset->timeToFreeze;
    mSparkDelay = xrand_GenRandFloat();
    sSparkAsset.num_sparks = 30.0f * (1.0f - ratio) + 10.0f;
    sSparkAsset.dir_vary = PI / 2;

    while (count-- > 0) {
        F32 yaw = 2.0f * PI * xrand_GenRandFloatLessThanOne() - PI;
        xVec3 dir;
        dir.assign(isin(yaw) * icos(PI / 4), isin(PI / 4), icos(yaw) * icos(PI / 4));

        xVec3 pos = *(xVec3*)&model->Mat->pos;
        if (mSparkRaised) {
            pos.y += 2.5f;
        }
        mSparkRaised = !mSparkRaised;

        zSparkEmitter::EmitSparks(sSparkAsset, pos, dir, dt, -1, -1, ISND_INVALID_SND_GROUP_HANDLE);
    }
}

void zFreezableObject::Explode()
{
    if (!mActive) {
        return;
    }

    xglobals->players.GetFrozone()->playStats.rangeDamage++;
    mHealth = 0.0f;
    mTimeToFreeze = 0.0f;

    if (mShrapnel) {
        mShrapnel->initCB(mShrapnel, model, NULL, NULL, NULL);
    }

    EmitIceShrapnel(false);
    Off();
    zEntEvent(this, this, eEventKilled);

    if (mKilledTarget) {
        zEntEvent(this, mKilledTarget, eEventKilled);
    }

    if (mRumble && mRumble->mDefaultSphericalEmitterAsset) {
        mRumble->UpdatePosition(mAsset->pos);
        zEntEvent(this, mRumble, eEventStartRumbleEffect);
    }
}

void zFreezableObject::EmitIceShrapnel(bool alwaysPlaySound)
{
    if (sIceShrap) {
        S32 count = 8.0f * (1.0f - mTimeToFreeze / mAsset->timeToFreeze);
        F32 step = count > 1 ? 2.5f / (count - 1) : 2.5f;
        xVec3 offset = xVec3::m_Null;

        for (S32 i = 0; i < count; i++) {
            sIceShrap->initCB(sIceShrap, model, &offset, NULL, NULL);
            offset.y += step;
        }
    }

    if (alwaysPlaySound || mTimeToFreeze / mAsset->timeToFreeze < 0.5f) {
        xSndMgrPlay(sSndIceShatter, 0, NULL, NULL, this, NULL, NULL);
    }
}

void zFreezableObject::Render(xEnt* ent)
{
    ((zFreezableObject*)ent)->Render();
}

void zFreezableObject::Render()
{
    if (mActive && model && !xModelSphereCull(iModelWorldBoundSphere(model->Data, model->Mat, NULL))) {
        xEntRender(this);

        if (mIceModel && mTimeToFreeze < mAsset->timeToFreeze && !mThawing) {
            xModelRender(mIceModel);
        }
    }
}

void zFreezableObject::EventCB(xBase*, xBase* to, U32 toEvent, const F32*, xBase*, U32)
{
    zFreezableObject* obj = (zFreezableObject*)to;

    switch (toEvent) {
    case eEventReset:
        obj->Reset();
        break;
    case eEventKill:
        obj->Explode();
        break;
    case eEventVisible:
    case eEventCollision_Visible_On:
        obj->On();
        break;
    case eEventInvisible:
    case eEventCollision_Visible_Off:
        obj->Off();
        break;
    case eEventEnable:
        xEntEnable(obj);
        break;
    case eEventDisable:
        xEntDisable(obj);
        break;
    case eEventOn:
        obj->mCanFreeze = true;
        break;
    case eEventOff:
        obj->mCanFreeze = false;
        break;
    case eEventHit:
        break;
    }
}
