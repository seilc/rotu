#include "xCommon.h"
#include "zAsync.h"

#include "iAsync.h"
#include "iCamera.h"
#include "iTime.h"
#include "iTRC.h"
#include "containers.h"
#include "xFont.h"
#include "xPad.h"
#include "zGlobals.h"
#include "zScene.h"
#include "zUIMgr.h"

namespace zAsync {

RwCamera* cam;
RpLight* light;
RwFrame* frame;
RpWorld* world;
Screen* screen;

void Init(Screen* scr)
{
    screen = scr;
}

void Allocate()
{
    cam = iCameraCreate(FB_XRES, FB_YRES, FALSE);

    light = RpLightCreate(rpLIGHTPOINT);
    frame = RwFrameCreate();
    RwBBox tmpbbox = { { 10.0f, 10.0f, 10.0f }, { -10.0f, -10.0f, -10.0f } };
    world = RpWorldCreate(&tmpbbox);

    RwV3d lightpos = { 5.0f, 5.0f, -5.0f };
    RwRGBAReal color = { 1.0f, 0.0f, 0.0f, 1.0f };

    RpLightSetFrame(light, frame);
    RpLightSetColor(light, &color);
    RwFrameTranslate(frame, &lightpos, rwCOMBINEREPLACE);
    RpWorldAddLight(world, light);

    RpWorldAddCamera(world, cam);

    if (screen) {
        screen->Init();
    }

    zUIMgrEnableHDR(false);
}

void Free()
{
    if (screen) {
        screen->Exit();
    }

    iAsyncEnd();
    GXDrawDone();

    _rwFrameSyncDirty();

    RpWorldRemoveLight(world, light);
    RwFrameDestroy(frame);
    RpLightDestroy(light);

    RpWorldRemoveCamera(world, cam);
    RpWorldDestroy(world);

    iCameraDestroy(cam);
    cam = NULL;
    light = NULL;
    frame = NULL;
    world = NULL;
}

void* RenderThread(void*)
{
    static F32 rot = 0.0f;

    iTime lastTime = iTimeGet();

    globals.inLoadingScreen = true;

    for (; !iAsyncIsDone();) {
        bool diskError = false;

        iAsyncBeginUpdate();

        {
            iTime curTime = iTimeGet();
            F32 dt = iTimeDiffSec(lastTime, curTime);
            if (dt == 0.0f) {
                dt = 0.0001f;
            }

            lastTime = curTime;

            RwRGBA bg = { 0, 0, 0, 0 };

            iAsyncLockRW();

            RwCamera* currentCamera = RwCameraGetCurrentCamera();

            diskError = iTRCDisk::CheckDiskErrorInHigherPriorityThread();

            if (currentCamera || xMemScratchInUse()) {
                iAsyncUnlockRW();
                iAsyncEndUpdate();

                if (diskError) {
                    iAsyncSuspend();
                }

                VIWaitForRetrace();
                continue;
            }

            if (cam && RwCameraGetRaster(cam)) {
                RwCameraClear(cam, &bg, rwCAMERACLEARIMAGE | rwCAMERACLEARZ);
                RwCameraBeginUpdateWrapper(cam);

                if (screen) {
                    if (globals.screen || cam) {
                        screen->Update(dt);
                        screen->Render();
                    } else {
                    }
                }

                RwCameraEndUpdate(cam);
                RwCameraShowRaster(cam, NULL, 1);
            }

            if (currentCamera) {
                RwCameraBeginUpdateWrapper(currentCamera);
            }

            iAsyncUnlockRW();
        }

        gFrameCount++;
        iAsyncEndUpdate();
    }

    globals.inLoadingScreen = false;

    return NULL;
}

} // namespace zAsync
