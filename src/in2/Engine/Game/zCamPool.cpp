#include "xCommon.h"
#include "zCamPool.h"

#include "xCam.h"

zCamPoolBase::zCamPoolBase() : mNumCameras(0), mNextCamera(0)
{
}

void zCamPoolBase::SceneExit()
{
    for (S32 i = 0; i < mNumCameras; i++)
        mCameras[i]->destroy();
    mNumCameras = 0;
}

void zCamPoolBase::AddCamera(xCam* newCam, const char* tweakPrefix)
{
    if (mNumCameras < MAX_POOL_CAMERAS) {
        if (newCam) {
            newCam->create();
            mCameras[mNumCameras++] = newCam;
        }
    }
}

xCam* zCamPoolBase::GetNextCamera()
{
    static S32 depth = 0;

    for (S32 i = 0; i < mNumCameras; i++) {
        xCam* cam = mCameras[mNextCamera++];
        if (mNextCamera >= mNumCameras)
            mNextCamera = 0;
        if (!cam->started())
            return cam;
    }

    return NULL;
}
