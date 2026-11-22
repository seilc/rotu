#ifndef ZCAMSPLINECOMMONMIX_H
#define ZCAMSPLINECOMMONMIX_H

#include "xSpringy.h"
#include "zCameraCurve.h"

class zCamSplineCommonMix
{
public:
    zCamSplineCommonMix();

    virtual void SetCurves(zCameraCurve* curveInfo);

    void InitPaths();
    void UpdatePaths(F32);

    static const F32 cDefaultCameraSpring;

protected:
    zCameraCurve* mCurveInfo;
    zCameraCurve::Follower mCurveFollower;
    xSpringyVec3 mCameraPathPosSprung;
};

#endif
