#include "xCommon.h"
#include "xRenderState.h"

#include "xMath.h"

#include <dolphin.h>

RwReal old_near_plane = -HUGE;
RwReal old_far_plane = -HUGE;

void xRenderSceneEnter()
{
}

void xRenderSceneExit()
{
}

void xRenderStateSetZBias(RwReal bias)
{
    RwCamera* cam = RwCameraGetCurrentCamera();
    if (!cam) return;

    static const RwReal bias_scale = 0.005f;
    bias *= bias_scale;

    old_near_plane = RwCameraGetNearClipPlane(cam);
    old_far_plane = RwCameraGetFarClipPlane(cam);

    RwCameraEndUpdate(cam);
    RwCameraSetNearClipPlane(cam, bias + old_near_plane);
    RwCameraSetFarClipPlane(cam, bias + old_far_plane);
    RwCameraBeginUpdate(cam);
}

void xRenderStateResetZBias()
{
    RwCamera* cam = RwCameraGetCurrentCamera();
    if (!cam) return;

    RwCameraEndUpdate(cam);
    RwCameraSetNearClipPlane(cam, old_near_plane);
    RwCameraSetFarClipPlane(cam, old_far_plane);
    RwCameraBeginUpdate(cam);

    old_near_plane = old_far_plane = -HUGE;
}

static void* old_alpha_discard_value_gc;

void xRenderStateSetAlphaDiscard(RwInt32 value)
{
    RwRenderStateGet(rwRENDERSTATEALPHATESTFUNCTIONREF, &old_alpha_discard_value_gc);

    RwGameCubeSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_GEQUAL, value);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_GEQUAL, value);

    if (value > 0) {
        _rwDlRenderStateSetZCompLoc(FALSE);
        GXSetZCompLoc(GX_FALSE);
    } else {
        _rwDlRenderStateSetZCompLoc(TRUE);
        GXSetZCompLoc(GX_TRUE);
    }
}

void xRenderStateResetAlphaDiscard()
{
    xRenderStateSetAlphaDiscard(0);
}

void xRenderFixUntexturedBegin(RpAtomic*)
{
}

void xRenderFixUntexturedEnd(RpAtomic*)
{
}

void xRenderFixIMBegin()
{
}

void xRenderFixIMEnd()
{
}

void xRenderStateBlendModesSet(RwBlendFunction srcBlend, RwBlendFunction destBlend)
{
    RwRenderStateSetMacro(rwRENDERSTATESRCBLEND, (void*)srcBlend);
    RwRenderStateSetMacro(rwRENDERSTATEDESTBLEND, (void*)destBlend);
}

void xRenderStateZModesSet(RwBool ZWrite, RwBool ZTest)
{
    RwRenderStateSetMacro(rwRENDERSTATEZWRITEENABLE, (void*)ZWrite);
    RwRenderStateSetMacro(rwRENDERSTATEZTESTENABLE, (void*)ZTest);
}

void xRenderStateBlendAndZModesSet(RwBlendFunction srcBlend, RwBlendFunction destBlend, RwBool ZWrite, RwBool ZTest)
{
    RwRenderStateSetMacro(rwRENDERSTATESRCBLEND, (void*)srcBlend);
    RwRenderStateSetMacro(rwRENDERSTATEDESTBLEND, (void*)destBlend);
    RwRenderStateSetMacro(rwRENDERSTATEZWRITEENABLE, (void*)ZWrite);
    RwRenderStateSetMacro(rwRENDERSTATEZTESTENABLE, (void*)ZTest);
}

void xRenderStateBlendModesGet(RwBlendFunction* srcBlend, RwBlendFunction* destBlend)
{
    if (srcBlend) {
        RwRenderStateGetMacro(rwRENDERSTATESRCBLEND, (void*)srcBlend);
    }

    if (destBlend) {
        RwRenderStateGetMacro(rwRENDERSTATEDESTBLEND, (void*)destBlend);
    }
}
