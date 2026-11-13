#include "xCommon.h"
#include "xSoundFX.h"

#include "zEvent.h"
#include "xstransvc.h"
#include "xRand.h"
#include "zMusic.h"
#include "zGame.h"
#include "zScene.h"
#include "xGlobals.h"

static void xSoundFXPlay(xSoundFX* t);
static void xSoundFXStop(xSoundFX* t);

static bool xSoundFXWillSendDone(xSoundFX* pSoundFX)
{
    if (pSoundFX->linkCount > 0) {
        const xLinkAsset* idx = pSoundFX->link;

        for (S32 i = 0; i < pSoundFX->linkCount; i++, idx++) {
            if (idx->srcEvent == eEventDone)
                return true;
        }
    }
    
    return false;
}

void xSoundFXInit(void* t, void* asset)
{
    xSoundFXInit((xSoundFX*)t, (xSoundFXAsset*)asset);
}

void xSoundFXInit(xSoundFX* t, xSoundFXAsset* asset)
{
    xBaseInit(t, asset);

    t->eventFunc = xSoundFXEventCB;
    t->asset = asset;

    if (!xSoundFXIsHandleSet(asset)) {
        t->asset->soundAsset = xSndMgrGetSoundGroup(t->asset->soundAssetID);
        if (asset->soundAsset != ISND_INVALID_SND_GROUP_HANDLE) {
            xSoundFXSetHandleSet(asset, true);
        }
    }

    if (t->linkCount > 0)
        t->link = (xLinkAsset*)((U8*)t->asset + sizeof(xSoundFXAsset));
    else
        t->link = NULL;

    if (xSoundFXWillSendDone(t)) {
        xSoundFXSetSendsDone(asset, true);
    }

    t->sndHandle = XSNDMGR_INVALID_SND_HANDLE;

    xSoundFXSetSoundFXPlaying(t->asset, false);
}

void xSoundFXReset(xSoundFX* ent)
{
    xBaseReset(ent, ent->asset);
}

void xSoundFXEventCB(xBase*, xBase* to, U32 toEvent, const F32* toParam, xBase*, U32)
{
    xSoundFX* t = (xSoundFX*)to;

    switch (toEvent) {
    case eEventReset:
    case eEventDebugReset:
        xSoundFXReset(t);
        break;
    case eEventPlay:
        xSoundFXSetSoundFXPlaying(t->asset, true);
        xSoundFXPlay(t);
        break;
    case eEventPlayMaybe:
        if (xrand_RandomChoice(100) >= toParam[0]) {
            xSoundFXSetSoundFXPlaying(t->asset, true);
            xSoundFXPlay(t);
        }
        break;
    case eEventStop:
        if (t->sndHandle != XSNDMGR_INVALID_SND_HANDLE) {
            xSoundFXSetSoundFXPlaying(t->asset, false);
            xSoundFXStop(t);
        }
        break;
    case eEventPause:
        while (true) {
        }
        break;
    case eEventChangeVolume:
    {
        F32 destVolume = toParam[0];
        if (destVolume < 0.0f || destVolume > 1.0f) {
            if (destVolume < 0.0f) destVolume = 0.0f; else if (destVolume > 1.0f) destVolume = 1.0f;
        }
        xSndMgrFade(t->sndHandle, toParam[1], destVolume);
        break;
    }
    case eEventFadeVolume:
    {
        F32 startVolume = toParam[0];
        if (startVolume < 0.0f || startVolume > 1.0f) {
            if (startVolume < 0.0f) startVolume = 0.0f; else if (startVolume > 1.0f) startVolume = 1.0f;
        }
        F32 destVolume = toParam[1];
        if (destVolume < 0.0f || destVolume > 1.0f) {
            if (destVolume < 0.0f) destVolume = 0.0f; else if (destVolume > 1.0f) destVolume = 1.0f;
        }
        xSndMgrFade(t->sndHandle, toParam[2], startVolume, destVolume);
        break;
    }
    }
}

static bool xSoundFXEnsureFreeAudioStream(bool willingToStopMainGameMusic)
{
    bool haveFreeStreamedVoice;

    if (haveFreeStreamedVoice = iSndGetFreeStreamInfo(0)) {
        return true;
    }

    xSndMgrStopPausedSounds(2);
    if (haveFreeStreamedVoice = iSndGetFreeStreamInfo(0)) {
        return true;
    }

    if (willingToStopMainGameMusic) {
        zMusicKill();
    }
    haveFreeStreamedVoice = iSndGetFreeStreamInfo(0);
    if (haveFreeStreamedVoice) {
        return true;
    }

    xSndMgrStopPausedSounds(3);
    if (haveFreeStreamedVoice = iSndGetFreeStreamInfo(0)) {
        return true;
    }

    return false;
}

static void xSoundFXPlay(xSoundFX* t)
{
    if (t->sndHandle != XSNDMGR_INVALID_SND_HANDLE) {
        if (!zGameIsPaused() || !xSndMgrIsDialogPlaying() || xSndMgr_GetGroup(t->asset->soundAsset)->header.eSoundCategory != 2) {
            xSndMgrStop(t->sndHandle);
        }
    }
    
    S32 sndFlags = 0;

    if (xSoundFXIsAttached(t->asset)) {
        xEnt* ent = (xEnt*)zSceneFindObject(t->asset->attachID);
        if (ent) {
            sndFlags |= 0x800;
            t->sndHandle = xSndMgrPlay(t->asset->soundAsset, sndFlags, xEntGetPos(ent), NULL, ent, NULL, NULL);
        }
    } else {
        if (!zGameIsPaused() || !xSndMgrIsDialogPlaying() || xSndMgr_GetGroup(t->asset->soundAsset)->header.eSoundCategory != 2) {
            if (!zGameIsPaused() || !xSndMgrIsStream(t->asset->soundAsset) || xSoundFXEnsureFreeAudioStream(true)) {
                t->sndHandle = xSndMgrPlay(t->asset->soundAsset, sndFlags, &t->asset->pos, NULL, NULL, NULL, NULL);
            }
        }
    }
}

static void xSoundFXStop(xSoundFX* t)
{
    xSndMgrStop(t->sndHandle);
}

xSoundFX* xSoundFXGet(U32 soundfxID)
{
    zScene* s = xglobals->sceneCur;
    S32 count = s->baseCount[eBaseTypeSoundFX];
    xSoundFX* sfx = NULL;
    for (S32 i = 0; i < count; i++) {
        sfx = (xSoundFX*)s->baseList[eBaseTypeSoundFX] + i;
        if (sfx->id == soundfxID)
            break;
    }
    return sfx;
}
