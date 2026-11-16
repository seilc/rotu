#include "xCommon.h"
#include "zFXBossBrain.h"

#include "xEnt.h"
#include "xFXHighDynamicRange.h"
#include "xIM.h"
#include "xMath.h"
#include "xModel.h"
#include "xModelBucket.h"
#include "xRand.h"
#include "xstransvc.h"
#include "zBase.h"
#include "zCineFXShared.h"
#include "zEvent.h"
#include "zGlobals.h"
#include "zNPC.h"
#include "zNPCBaseIN2.h"
#include "zNPCMgr.h"
#include "zRenderState.h"
#include "zScene.h"

namespace {

RwRaster* sCinFXBrainRaster;
xModelInstance* sCinFXDomeModel;
F32 sCinFXScale;
xVec3 sCinFXPos;
S32 sCinFXNumCards;
bool sCinFXUsed;
zFXBossBrain::card sCinFXCardList[MAX_BRAIN_CARDS];

} // namespace

void zFXBossBrain::Init(xBase& data, xDynAsset& asset, size_t)
{
    ((zFXBossBrain&)data).Init((zFXBossBrainAsset*)&asset);
}

void zFXBossBrain::Init(zFXBossBrainAsset* asset)
{
    mAsset = asset;
    xBaseInit(this, asset);
    eventFunc = Event;

    if (linkCount) {
        link = (xLinkAsset*)((U8*)asset + sizeof(zFXBossBrainAsset));
    }

    RwTexture* tex = asset->raster_id ? (RwTexture*)xSTFindAsset(asset->raster_id, NULL) : NULL;
    if (tex) {
        mRaster = RwTextureGetRaster(tex);
    }

    if (asset->dome_id) {
        RpAtomic* atomic = (RpAtomic*)xSTFindAsset(asset->dome_id, NULL);
        mDome = atomic ? xModelInstanceAlloc(atomic, NULL, 0, 0, NULL) : NULL;
        if (mDome) {
            mDome->modelID = asset->dome_id;
        }
    }

    mScale = asset->scale;
}

void zFXBossBrain::Setup()
{
    Reset();
}

void zFXBossBrain::Reset()
{
    mNumCards = mAsset->num_cards;
    mCardTime = mAsset->card_time;
    mOn = mAsset->flags & k_ZFXBOSSBRAIN_START_ON;

    if (mAsset->attach_id) {
        mAttachEnt = (xEnt*)zSceneFindObject(mAsset->attach_id);
        AttachTo(mAttachEnt, mAsset->attach_bone);
    } else {
        mAttachEnt = NULL;
        mPos = mAsset->pos;
    }

    for (S32 i = 0; i < mNumCards; i++) {
        randomizeCard(mCards[i], mCardTime, true);
    }
}

void zFXBossBrain::randomizeCard(card& c, F32 time, bool full)
{
    bool reset = full || xrand_GenRandFloat() < 0.5f;

    if (reset) {
        F32 yaw = 2.0f * PI * xrand_GenRandFloatLessThanOne() - PI;
        F32 pitch = 2.0f * PI * xrand_GenRandFloatLessThanOne() - PI;
        F32 roll = 2.0f * PI * xrand_GenRandFloatLessThanOne() - PI;
        xMat3x3Euler(&c.mat, yaw, pitch, roll);
        c.flags = xrand_RandomInt32() & k_ZFXBOSSBRAINCARD_ALL;
    } else {
        c.flags = (c.flags & ~k_ZFXBOSSBRAINCARD_QUARTER) | (xrand_RandomInt32() & k_ZFXBOSSBRAINCARD_QUARTER);
    }

    c.time = time * (1.0f - 0.5f * xrand_GenRandFloat());
}

void zFXBossBrain::AttachBaseTo(xBase* base, S32 bone)
{
    if (base && base->baseType == eBaseTypeNPC) {
        AttachTo(zNPCMgrSelf()->FindNPC(base->id)->get_data(), bone);
    } else {
        AttachTo((xEnt*)base, bone);
    }
}

void zFXBossBrain::AttachTo(xEnt* ent, S32 bone)
{
    mAttachEnt = ent;
    mAttachBone = bone;
}

void zFXBossBrain::UpdateAll(F32 dt)
{
    zScene* s = globals.sceneCur;
    if (s->baseCount[eBaseTypeBossBrain] > 0) {
        zFXBossBrain* it = (zFXBossBrain*)s->baseList[eBaseTypeBossBrain], *end = it + s->baseCount[eBaseTypeBossBrain];
        for (; it != end; it++) {
            it->Update(dt);
        }
    }
}

void zFXBossBrain::Update(F32 dt)
{
    if (!mOn) {
        return;
    }

    if (mAttachEnt) {
        xMat4x3 mat;
        RwMatrix* bones = mAttachEnt->model->Mat;
        if (mAttachBone) {
            xMat4x3Mul(&mat, (xMat4x3*)&bones[mAttachBone], (xMat4x3*)bones);
        } else {
            mat = *(xMat4x3*)bones;
        }

        xMat3x3RMulVec(&mPos, &mat, &mAsset->pos);
        xVec3AddTo(&mPos, &mat.pos);

        if (mDome) {
            *mDome->Mat = *(RwMatrix*)&mat;
        }
    }

    updateCards(mCards, mNumCards, mCardTime, dt);
}

void zFXBossBrain::updateCards(card* cards, S32 count, F32 time, F32 dt)
{
    for (S32 i = 0; i < count; i++) {
        cards[i].time -= dt;
        if (cards[i].time < 0.0f) {
            randomizeCard(cards[i], time, false);
        }
    }
}

void zFXBossBrain::EarlyRenderAll()
{
    zScene* s = globals.sceneCur;
    if (s->baseCount[eBaseTypeBossBrain] > 0) {
        zFXBossBrain* it = (zFXBossBrain*)s->baseList[eBaseTypeBossBrain], *end = it + s->baseCount[eBaseTypeBossBrain];
        for (; it != end; it++) {
            if (it->mOn && it->mDome) {
                xModelBucket_Add(it->mDome, false);
            }
        }
    }
}

void zFXBossBrain::RenderAll()
{
    if (sCinFXUsed) {
        render(sCinFXCardList, sCinFXNumCards, sCinFXPos, sCinFXScale, sCinFXBrainRaster);
    }

    zScene* s = globals.sceneCur;
    if (s->baseCount[eBaseTypeBossBrain] > 0) {
        SDRenderState oldState = zRenderStateCurrent();
        zRenderState(eRenderStateBossBrain);
        bool bright = xFXHighDynamicRangeIsBright();
        xFXHighDynamicRangeBrighten(true, false);

        zFXBossBrain* it = (zFXBossBrain*)s->baseList[eBaseTypeBossBrain], *end = it + s->baseCount[eBaseTypeBossBrain];
        for (; it != end; it++) {
            it->Render();
        }

        xFXHighDynamicRangeBrighten(bright, false);
        zRenderState(oldState);
    }
}

void zFXBossBrain::Render()
{
    if (!mOn || mNumCards <= 0) {
        return;
    }

    render(mCards, mNumCards, mPos, mScale, mRaster);
}

void zFXBossBrain::render(const card* cards, S32 count, const xVec3& pos, F32 scale, RwRaster* raster)
{
    xIMLockContext lock;
    xIMLock(lock, XIM_TRILIST, count * 6);

    xVec3* vert = lock.pos;
    xVec2* uv = lock.uv;
    U32* color = (U32*)lock.color;

    for (S32 i = 0; i < count; i++) {
        const card& c = cards[i];

        xVec3 p0, p1, p2, p3;
        xVec3 a = { -scale, scale, 0.0f };
        xVec3 b = { scale, scale, 0.0f };
        xMat3x3RMulVec(&p0, &c.mat, &a);
        xMat3x3RMulVec(&p1, &c.mat, &b);
        p3.Sub(pos, p0);
        p2.Sub(pos, p1);
        p0.Add(pos);
        p1.Add(pos);

        vert[0] = p0;
        vert[1] = p1;
        vert[2] = p2;
        vert[3] = p2;
        vert[4] = p1;
        vert[5] = p3;
        vert += 6;

        color[0] = 0xFFFFFFFF;
        color[1] = 0xFFFFFFFF;
        color[2] = 0xFFFFFFFF;
        color[3] = 0xFFFFFFFF;
        color[4] = 0xFFFFFFFF;
        color[5] = 0xFFFFFFFF;
        color += 6;

        F32 u = (c.flags & k_ZFXBOSSBRAINCARD_FLIP_U) ? 0.5f : 0.0f;
        F32 v = (c.flags & k_ZFXBOSSBRAINCARD_FLIP_V) ? 0.5f : 0.0f;
        F32 du = (c.flags & k_ZFXBOSSBRAINCARD_U_HALF) ? 0.5f : 0.0f;
        F32 dv = (c.flags & k_ZFXBOSSBRAINCARD_V_HALF) ? 0.5f : 0.0f;
        F32 u0 = u + du;
        F32 v0 = v + dv;
        F32 u1 = (0.5f - u) + du;
        F32 v1 = (0.5f - v) + dv;

        uv[0].assign(u0, v0);
        uv[1].assign(u1, v0);
        uv[2].assign(u0, v1);
        uv[3].assign(u0, v1);
        uv[4].assign(u1, v0);
        uv[5].assign(u1, v1);
        uv += 6;
    }

    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, raster);
    xIMRender(lock, NULL, 0);
    xIMUnlock(lock);
}

void zFXBossBrain::Event(xBase* from, xBase* to, U32 toEvent, const F32* toParam, xBase* toParamWidget,
                         U32 toParamWidgetID)
{
    ((zFXBossBrain*)to)->Event(from, toEvent, toParam, toParamWidget, toParamWidgetID);
}

void zFXBossBrain::Event(xBase* from, U32 toEvent, const F32* toParam, xBase* toParamWidget, U32 toParamWidgetID)
{
    switch (toEvent) {
    case eEventReset:
    case eEventDebugReset:
        Reset();
        break;
    case eEventSetFrequency:
        mCardTime = toParam[0];
        break;
    case eEventSetIntensity: {
        S32 count = (S32)toParam[0];
        for (S32 i = mNumCards; i < count; i++) {
            randomizeCard(mCards[i], 1.0f, false);
        }
        mNumCards = count;
        break;
    }
    case eEventOn:
        mOn = true;
        break;
    case eEventOff:
        mOn = false;
        break;
    case eEventAttachTo:
        AttachBaseTo(toParamWidget, (S32)toParam[0]);
        break;
    }
}

void zFXBossBrain::cinfxUpdate(const zCutsceneMgr*, NCINEntry* fxrec, S32 killit)
{
    cinfx_bossbrain_context* ctx = (cinfx_bossbrain_context*)fxrec->input.fxparm;
    sCinFXScale = ctx->scale;
    sCinFXNumCards = ctx->num_cards;

    if (fxrec->flg_stat & 0x4) {
        RwTexture* tex = (RwTexture*)xSTFindAsset(ctx->raster_id, NULL);
        sCinFXBrainRaster = tex ? RwTextureGetRaster(tex) : NULL;

        if (ctx->dome_id) {
            RpAtomic* atomic = (RpAtomic*)xSTFindAsset(ctx->dome_id, NULL);
            sCinFXDomeModel = atomic ? xModelInstanceAlloc(atomic, NULL, 0, 0, NULL) : NULL;
            if (sCinFXDomeModel) {
                sCinFXDomeModel->modelID = ctx->dome_id;
            }
        }

        for (S32 i = 0; i < ctx->num_cards; i++) {
            randomizeCard(sCinFXCardList[i], ctx->card_time, true);
        }
    }

    if (killit) {
        fxrec->flg_stat |= 0x2;
        if (sCinFXDomeModel) {
            xModelInstanceFree(sCinFXDomeModel, true);
            sCinFXDomeModel = NULL;
        }
    }

    updateCards(sCinFXCardList, ctx->num_cards, ctx->card_time, 1.0f / 30.0f);
}

void zFXBossBrain::cinfxAnimate(const zCutsceneMgr*, NCINEntry* fxrec, RpAtomic*, RwMatrix* animMat, U32, U32, U32&)
{
    const xMat4x3& root_mat = *(const xMat4x3*)animMat;
    const xMat4x3& bone_mat = *(const xMat4x3*)&animMat[fxrec->index.idx_bone];
    xMat4x3 mat;
    if (fxrec->index.idx_bone == 0) {
        mat = root_mat;
    } else {
        xMat4x3Mul(&mat, &bone_mat, &root_mat);
    }

    xMat4x3ToWorld(&sCinFXPos, &mat, &fxrec->basic.pos_A[0]);

    cinfx_bossbrain_context* ctx = (cinfx_bossbrain_context*)fxrec->input.fxparm;
    if (sCinFXDomeModel) {
        xMat4x3 domeMat = mat;
        if (ctx->dome_scale) {
            xMat3x3MulScaleC(domeMat, ctx->dome_scale);
        }

        xVec3 offset = { 0.0f, ctx->dome_height, 0.0f };
        xVec3 domePos;
        xMat4x3ToWorld(&domePos, &domeMat, &offset);

        *sCinFXDomeModel->Mat = *(RwMatrix*)&domeMat;
        *(xVec3*)&sCinFXDomeModel->Mat->pos = domePos;
        xModelBucket_Add(sCinFXDomeModel, false);
    }
}

void zFXBossBrain::cinfxRender(const zCutsceneMgr*, NCINEntry*)
{
}

void zFXBossBrain::cinfxAddTweaks(const char*, NCINEntry*)
{
}
