#ifndef ZCAMPOOL_H
#define ZCAMPOOL_H

#include "types.h"

struct xCam;

#define MAX_POOL_CAMERAS 4

class zCamPoolBase
{
public:
    zCamPoolBase();

    void SceneExit();
    void AddCamera(xCam* newCam, const char* tweakPrefix);
    xCam* GetNextCamera();

private:
    xCam* mCameras[MAX_POOL_CAMERAS];
    S32 mNumCameras;
    S32 mNextCamera;
};

#endif
