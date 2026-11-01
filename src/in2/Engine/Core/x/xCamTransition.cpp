#include "xCamTransition.h"

#include "xCamTransitionAsset.h"
#include "xEvent.h"
#include "xSpacePartitionTree.h"
#include "xSplineAsset.h"
#include "xutil.h"
#include "zBase.h"
#include "zCam.h"
#include "zCameraCurve.h"
#include "zCamMarker.h"
#include "zEntTrigger.h"
#include "zEvent.h"
#include "zGlobals.h"
#include "zPlayer.h"
#include "zScene.h"

inline F32 xCamPathBias::RawBias()
{
    return mBias;
}

void xCamTransition::Init(const xCamTransitionAsset& asset)
{
    asset.Validate();

    xBaseInit(this, &asset);

    mAsset = &asset;
    mType = asset.mType;
    eventFunc = zCamTransition_EventCB;
    link = asset.Links();
    mActive = false;
    mCamBlendBias = NULL;
    mTransitionVolume = NULL;
}

void xCamTransition::Init(zEntTrigger& theVolume)
{
    id = xStrHashCat(theVolume.id, "_PATHTRANS");
    baseType = eBaseTypeCamTransitionPath;
    baseFlags = 0;
    linkCount = 0;
    link = NULL;
    mAsset = NULL;
    eventFunc = zCamTransition_EventCB;
    mType = 1;
    mCamBlendBias = new (eMemStaticType_0, eMemMgrTag_Camera) xCamPathBias(&theVolume);
    mTransitionVolume = &theVolume;
}

void xCamTransition::Init(xBase& data, xDynAsset& asset, size_t)
{
    ((xCamTransition&)data).Init((xCamTransitionAsset&)asset);
}

void xCamTransition::AddTriggerTransition(zEntTrigger& theVolume)
{
    xCamTransition* trans = new (eMemStaticType_0, eMemMgrTag_Camera) xCamTransition();

    if (trans) {
        trans->Init(theVolume);
        theVolume.camTransition = trans;
    }
}

F32 xCamTransition::BlendTime() const
{
    return mAsset->mTime;
}

bool xCamTransition::IsCut()
{
    return (mType == 1) ? false : (BlendTime() <= 0.0f);
}

void xCamTransition::GetParams(xCamTransitionParams& params)
{
    switch (mType) {
    case 0:
        params.blend_time = mAsset->mTime;
        break;
    case 1:
        params.blend_time = 0.0f;
        break;
    }

    params.priority = 129;
    params.mTransitionObject = this;
}

void xCamTransition::Start()
{
    xBase* destCamera = NULL;

    switch (mType) {
    case 0:
        destCamera = zSceneFindObject(mAsset->mDestCameraID);
        break;
    case 1: {
        xBase* cameraA = zSceneFindObject(mTransitionVolume->Asset().camTransition.cameraAID);
        xBase* cameraB = zSceneFindObject(mTransitionVolume->Asset().camTransition.cameraBID);
        if (!cameraA || !cameraB) {
            return;
        }

        if (!mTransitionVolume->PointIntersects(zCamTransitionTarget())) {
            return;
        }

        bool forward = (((xCamPathBias*)mCamBlendBias)->GetInitialRawBias() < 0.5f);
        ((xCamPathBias*)mCamBlendBias)->SetDirection(forward);

        destCamera = forward ? cameraB : cameraA;
        
        if (globals.cam->primary && destCamera->id == globals.cam->primary->owner) {
            return;
        }
        
        break;
    }
    default:
        return;
    }
    
    if (!destCamera) {
        return;
    }

    xCamTransitionParams params;
    GetParams(params);

    switch (destCamera->baseType) {
    case eBaseTypeCameraCurve:
        mActive = ((zCameraCurve*)destCamera)->Activate(&params, false);
        break;
    case eBaseTypeCamBinaryPOI:
        mActive = ((zCamBinaryPOI*)destCamera)->Activate(&params, false);
        break;
    case eBaseTypeCamera:
        break;
    }
}

void xCamTransition::Completed(bool sendCompletionEvents)
{
    mActive = false;

    if (sendCompletionEvents) {
        switch (mType) {
        case 0:
            zEntEvent(this, eEventCameraTransitionDone);
            break;
        case 1: {
            F32 bias = ((xCamPathBias*)mCamBlendBias)->RawBias();
            zEntEvent(mTransitionVolume, bias < 0.5f ? eEventCameraTransitionToADone : eEventCameraTransitionToBDone);
            break;
        }
        }
    }
    
}

void zCamTransition_EventCB(xBase*, xBase* to, U32 toEvent, const F32*, xBase*, U32)
{
    switch (toEvent) {
    case eEventCameraTransitionBegin:
        ((xCamTransition*)to)->Start();
        break;
    case eEventEnable:
    case eEventDisable:
        break;
    }
}

xCamPathBias::xCamPathBias(zEntTrigger* triggerEnt)
    : xCamBias()
{
    mCurve = NULL;
    mTrigger = triggerEnt;
    mIsActive = false;
    mIsBiDirectional = true;

    mBias.SetTension(15.0f);

    ResetCurve();
}

void xCamPathBias::ResetCurve()
{
    mCurve = (const xNurbs*)xSTFindAsset(mTrigger->Asset().camTransition.curveID, NULL);
    
    if (!mCurve) {
        return;
    }

    mCurve->evaluate(0.0f, mCurveEnd0);
    mCurve->evaluate(1.0f, mCurveEnd1);
    
    xVec3 normal;
    normal.Sub(mCurveEnd1, mCurveEnd0);
    normal.y = 0.0f;
    normal.normalize();

    F32 dotAt = normal.dot(mTrigger->frame->mat.at);
    F32 dotRight = normal.dot(mTrigger->frame->mat.right);
    if (xabs(dotAt) > xabs(dotRight)) {
        mMappingPlaneNormal = mTrigger->frame->mat.at;
        if (dotAt < 0.0f) {
            mMappingPlaneNormal.negate();
        }
    } else {
        mMappingPlaneNormal = mTrigger->frame->mat.right;
        if (dotRight < 0.0f) {
            mMappingPlaneNormal.negate();
        }
    }

    F32 curveLength = mCurve->approximate_length(mCurve->numKnots() * 4);

    mGuessSpanU = 1.0f / curveLength;
}

void xCamPathBias::Start()
{
    mReversed = 0;
    mLastU = HUGE;
    CurveIntersectPlane(zCamTransitionTarget(), mLastU, mLastU);
    mBias.SnapTo(mLastU);
    mIsActive = true;
}

void xCamPathBias::Update(F32 dt)
{
    const xVec3& testPt = zCamTransitionTarget();

    if (mIsActive) {
        CurveIntersectPlane(testPt, mLastU, mLastU);
        mBias = mLastU;
        mBias.Update(dt);
    }

    mIsActive = (mTrigger->PointIntersects(testPt) || xabs(mBias.Goal() - mBias.Sprung()) > EPSILON);
}

void xCamPathBias::Reverse()
{
    mReversed ^= 1;
}

F32 xCamPathBias::GetInitialRawBias()
{
    F32 rawBias;
    CurveIntersectPlane(zCamTransitionTarget(), rawBias, HUGE);
    return rawBias;
}

void xCamPathBias::SetDirection(bool forward)
{
    mDirection = forward ? 0 : 1;
}

F32 xCamPathBias::GetBias()
{
    return (mReversed ^ mDirection) ? 1.0f - (F32)mBias : (F32)mBias;
}

bool xCamPathBias::IsActive()
{
    return mIsActive;
}

bool xCamPathBias::DidComplete()
{
    if (!mIsActive && mBias.Goal() > 0.0f && mBias.Goal() < 1.0f) {
    }

    return !mIsActive;
}

F32 xCamPathBias::CurveIntersectPlane(const xVec3& planePoint, F32& u, F32 guess)
{
    xPlane plane;
    plane.Init(mMappingPlaneNormal, planePoint);

    F32 distLo = plane.Dist(mCurveEnd0);
    F32 distHi = plane.Dist(mCurveEnd1);
    if ((distLo < 0.0f ? 1 : 0) == (distHi < 0.0f ? 1 : 0)) {
        if (distLo < 0.0f) {
            u = 1.0f;
        } else {
            u = 0.0f;
        }
        return u;
    }

    xVec3 loPt, hiPt;
    F32 loU, hiU;
    if (guess > HUGE/2) {
        loPt = mCurveEnd0;
        hiPt = mCurveEnd1;
        loU = 0.0f;
        hiU = 1.0f;
    } else {
        loU = range_limit(guess - mGuessSpanU, 0.0f, 1.0f);
        hiU = range_limit(guess + mGuessSpanU, 0.0f, 1.0f);
        mCurve->evaluate(loU, loPt);
        mCurve->evaluate(hiU, hiPt);
        distLo = plane.Dist(loPt);
        distHi = plane.Dist(hiPt);

        if ((distLo < 0.0f ? 1 : 0) == (distHi < 0.0f ? 1 : 0) && distLo != 0.0f && distHi != 0.0f) {
            loPt = mCurveEnd0;
            hiPt = mCurveEnd1;
            loU = 0.0f;
            hiU = 1.0f;
        }
    }

    xVec3 segmentV;
    segmentV.Sub(hiPt, loPt);

    F32 segmentLen2 = segmentV.length2();
    while (segmentLen2 > 0.0625f) {
        guess = 0.5f * (loU + hiU);

        xVec3 guessPt;
        mCurve->evaluate(guess, guessPt);

        F32 guessDist = plane.Dist(guessPt);
        if (guessDist < 0.0f) {
            loU = guess;
            loPt = guessPt;
        } else if (guessDist > 0.0f) {
            hiU = guess;
            hiPt = guessPt;
        } else {
            u = guess;
            return u;
        }

        segmentV.Sub(hiPt, loPt);
        segmentLen2 = segmentV.length2();
    }
    
    F32 t = -(plane.norm.dot(loPt) - plane.d) / plane.norm.dot(segmentV);
    u = t * (hiU - loU) + loU;
    return u;
}
