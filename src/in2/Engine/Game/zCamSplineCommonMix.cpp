#include "xCommon.h"
#include "zCamSplineCommonMix.h"

#include "zGlobals.h"

const F32 zCamSplineCommonMix::cDefaultCameraSpring = 10.0f;

zCamSplineCommonMix::zCamSplineCommonMix()
{
}

void zCamSplineCommonMix::SetCurves(zCameraCurve* curveInfo)
{
    mCurveInfo = curveInfo;
    InitPaths();
}

void zCamSplineCommonMix::InitPaths()
{
    mCurveFollower.Init(*mCurveInfo, globals.cameraPlayersMidpoint);
    mCameraPathPosSprung.SetGoal(mCurveFollower.CameraPathPos());
    mCameraPathPosSprung.Reset();
}

void zCamSplineCommonMix::UpdatePaths(F32)
{
    mCurveFollower.UpdatePosition(globals.cameraPlayersMidpoint);
}
