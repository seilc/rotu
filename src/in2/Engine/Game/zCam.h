#ifndef ZCAM_H
#define ZCAM_H

#include "zCamPlayer.h"

xVec3& zCamTransitionTarget();
zCamPlayer* zCamGetDefault();
xCam* zCamGetPrimary();
void zCamAdd(xCam& cam, bool force_cut);
void zCamRemove(xCam& cam, bool force_cut);
xMat4x3& zCamGetMatrix();

inline void zCamTrackGround(bool track)
{
    static bool sTrackGround = track;
}

#endif
