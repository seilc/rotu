#include "xCommon.h"
#include "zCamTargetSpline.h"

#include "zGlobals.h"
#include "zPlayerConstrainer.h"

void zCamTargetSpline::create()
{
    zCam2Player::create();
    cfg_common.priority = 0x81;
}

void zCamTargetSpline::stop()
{
    zCam2Player::stop();
    mCurveInfo->Stop(*this);
}

void zCamTargetSpline::start()
{
    zCam2Player::start();
    InitPaths();
    ApplyBeadValues(false);
    start_position_aim();
    mYOffset.Reset();
    if (mYOffset.Goal() != 0.0f) {
        coll_spatial.coord.cart.y += mYOffset.Goal();
        spatial.coord.cart.y = coll_spatial.coord.cart.y;
        refresh_mat();
    }

    mCurveInfo->Start(*this);
}

void zCamTargetSpline::pre_update(xScene&)
{
    oldSpatial = spatial;
    oldMat = mat;
}

void zCamTargetSpline::post_update(xScene&)
{
}

void zCamTargetSpline::ApplyBeadValues(bool lazy)
{
    const zCameraCurve::BeadInfo& beadInfo = mCurveFollower.InterpBeadInfo(false);

    set_secondary_target(mCameraPathPosSprung, beadInfo.mTargetRadius, (PI / 180.0f) * beadInfo.mTargetMarginAngle);

    mYOffset = mCurveFollower.InterpBeadInfo(false).mYOffset;

    rest_config newRest;
    get_rest_default(newRest);
    newRest.dist *= beadInfo.mDistanceScale;
    newRest.yaw_offset += (PI / 180.0f) * beadInfo.mLeadOffset;
    newRest.input_center_phi += (PI / 180.0f) * beadInfo.mPitchOffset;
    set_rest(newRest, lazy, 0.0f);

    F32 nearWall, farWall;
    zPlayerConstrainer::GetDefaults(nearWall, farWall);
    nearWall *= beadInfo.mNearWallScale;
    farWall *= beadInfo.mFarWallScale;
    zPlayerConstrainer::SetConstraints(nearWall, farWall);
}

void zCamTargetSpline::update(xScene& scene, F32 dt)
{
    UpdatePaths(dt);
    mCameraPathPosSprung.SetGoal(mCurveFollower.CameraPathPos());
    mCameraPathPosSprung.Update(dt);

    ApplyBeadValues(true);
    zCam2Player::update(scene, dt);
    if (mYOffset.Goal() != 0.0f || xabs(mYOffset) > EPSILON) {
        mYOffset.Update(dt);
        spatial.coord.cart.y += mYOffset;
        coll_spatial.coord.cart.y += mYOffset.Goal();
    }

    refresh_mat();
}
