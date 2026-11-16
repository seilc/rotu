#include "xCommon.h"
#include "xFMV.h"

#include "xSndMgr.h"
#include "zGame.h"
#include "zGlobals.h"
#include "zScene.h"
#include "xutil.h"
#include "zMenu.h"

#include <bink.h>

static void DefaultFMVPauseSoundCB(bool doPause);

static F32 sFMVVolumeScale = 1.0f;
static xFMVPauseSoundCallback sxFMVPauseSoundCB = DefaultFMVPauseSoundCB;

void xFMVPauseSound(bool doPause)
{
    sxFMVPauseSoundCB(doPause);

    if (doPause)
        xSndMgrUpdate(0.0f);
}

static void DefaultFMVPauseSoundCB(bool doPause)
{
    if (zGameIsPaused()) {
        xSndMgrPauseSounds(xSndCat_UI, doPause, true);

        if (globals.sceneCur->sceneID == IDTAG('M','N','U','S')) {
            if (zMenuRunning()) {
                xSndMgrPauseSounds(xSndCat_ALL, doPause, true);
            }
        }

        if (doPause) {
            xSndMgrStopSounds(xSndCat_DIALOG, true, false);
        }
    } else {
        xSndMgrPauseSounds(xSndCat_ALL, doPause, true);
    }
}

S32 xFMVGetBinkCompliantVolume()
{
    S32 volume = (S32)(32768.0f * xSndMixerGetOutputVolume(xSndCat_CUTSCENE) * sFMVVolumeScale);
    return volume;
}

RwTexture* xFMVGetTexture(MOVIE* pMovie, U16 looping, U16 freezeframed, U32 startframe, U32 endframe, U32)
{
    if (!pMovie) {
        return NULL;
    }

    if (!pMovie->hBink) {
        return NULL;
    }

    if (endframe == 0) {
        endframe = pMovie->hBink->Frames;
    }

    if (pMovie->hBink->LastFrameNum == 0xFFFFFFFF) {
        BinkGoto(pMovie->hBink, startframe + 1, 0);
    }

    RwRaster* pBinkRaster = RwTextureGetRaster(pMovie->pBinkTexture);

    if (!BinkWait(pMovie->hBink)) {
        BinkDoFrame(pMovie->hBink);

        RwUInt8* pBits = RwRasterLock(pBinkRaster, 0, rwRASTERLOCKWRITE | rwRASTERLOCKRAW);

        RwInt32 format = RwRasterGetFormat(pBinkRaster);
        RwInt32 pixformat = format & rwRASTERFORMATPIXELFORMATMASK;

        RwInt32 nDestPitch = RwRasterGetStride(pBinkRaster);

        S32 bFailed = BinkCopyToBuffer(pMovie->hBink, pBits, nDestPitch, RwRasterGetHeight(pBinkRaster), 0, 0, BINKCOPYALL | BINKSURFACE32);
        if (bFailed)
            return NULL;

        RwRasterUnlock(pBinkRaster);

        if (pMovie->hBink->Paused != freezeframed) {
            BinkPause(pMovie->hBink, freezeframed);
        }

        if (looping) {
            if (endframe == pMovie->hBink->LastFrameNum) {
                if (startframe != 0) {
                    BinkGoto(pMovie->hBink, startframe, 0);
                }
            }
            BinkNextFrame(pMovie->hBink);
            BinkWait(pMovie->hBink);
        }
        else if (endframe != pMovie->hBink->LastFrameNum) {
            BinkNextFrame(pMovie->hBink);
            BinkWait(pMovie->hBink);
        }
    }

    return pMovie->pBinkTexture;
}
