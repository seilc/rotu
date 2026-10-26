#include "xRenderState.h"

#include "xMath.h"
#include "xDebug.h"

#include <dolphin.h>

extern "C" {
extern void _rwDlRenderStateSetZCompLoc(RwBool zBeforeTex);
extern void RwGameCubeSetAlphaCompare(RwInt32 comp0, RwUInt8 ref0, RwInt32 op, RwInt32 comp1, RwUInt8 ref1);
}

#undef RwCameraBeginUpdate

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
    xASSERT_L(182, (old_near_plane == -HUGE) && (old_far_plane == -HUGE));

    RwCamera* cam = RwCameraGetCurrentCamera();
    xASSERT_L(184, cam != 0);
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
    xASSERT_L(202, (old_near_plane != -HUGE) && (old_far_plane != -HUGE));

    RwCamera* cam = RwCameraGetCurrentCamera();
    xASSERT_L(204, cam != 0);
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
    RwBool ret;

    ret = RwRenderStateSetMacro(rwRENDERSTATESRCBLEND, (void*)srcBlend);
    if (!ret) {
        xFAIL_FMT_L(2160, "Unable to set rwRENDERSTATESRCBLEND with value %d", srcBlend);
    }

    ret = RwRenderStateSetMacro(rwRENDERSTATEDESTBLEND, (void*)destBlend);
    if (!ret) {
        xFAIL_FMT_L(2166, "Unable to set rwRENDERSTATEDESTBLEND with value %d", destBlend);
    }
}

void xRenderStateZModesSet(RwBool ZWrite, RwBool ZTest)
{
    RwBool ret;

    ret = RwRenderStateSetMacro(rwRENDERSTATEZWRITEENABLE, (void*)ZWrite);
    if (!ret) {
        xFAIL_FMT_L(2176, "Unable to set rwRENDERSTATEZWRITEENABLE with value %d", ZWrite);
    }

    ret = RwRenderStateSetMacro(rwRENDERSTATEZTESTENABLE, (void*)ZTest);
    if (!ret) {
        xFAIL_FMT_L(2182, "Unable to set rwRENDERSTATEZTESTENABLE with value %d", ZTest);
    }
}

void xRenderStateBlendAndZModesSet(RwBlendFunction srcBlend, RwBlendFunction destBlend, RwBool ZWrite, RwBool ZTest)
{
    RwBool ret;

    ret = RwRenderStateSetMacro(rwRENDERSTATESRCBLEND, (void*)srcBlend);
    if (!ret) {
        xFAIL_FMT_L(2193, "Unable to set rwRENDERSTATESRCBLEND with value %d", srcBlend);
    }

    ret = RwRenderStateSetMacro(rwRENDERSTATEDESTBLEND, (void*)destBlend);
    if (!ret) {
        xFAIL_FMT_L(2199, "Unable to set rwRENDERSTATEDESTBLEND with value %d", destBlend);
    }

    ret = RwRenderStateSetMacro(rwRENDERSTATEZWRITEENABLE, (void*)ZWrite);
    if (!ret) {
        xFAIL_FMT_L(2205, "Unable to set rwRENDERSTATEZWRITEENABLE with value %d", ZWrite);
    }

    ret = RwRenderStateSetMacro(rwRENDERSTATEZTESTENABLE, (void*)ZTest);
    if (!ret) {
        xFAIL_FMT_L(2211, "Unable to set rwRENDERSTATEZTESTENABLE with value %d", ZTest);
    }
}

void xRenderStateBlendModesGet(RwBlendFunction* srcBlend, RwBlendFunction* destBlend)
{
    RwBool ret;

    if (srcBlend) {
        ret = RwRenderStateGetMacro(rwRENDERSTATESRCBLEND, (void*)srcBlend);
        if (!ret) {
            xFAIL_FMT_L(2225, "Unable to get rwRENDERSTATESRCBLEND with value %d", srcBlend);
        }
    }

    if (destBlend) {
        ret = RwRenderStateGetMacro(rwRENDERSTATEDESTBLEND, (void*)destBlend);
        if (!ret) {
            xFAIL_FMT_L(2234, "Unable to get rwRENDERSTATEDESTBLEND with value %d", destBlend);
        }
    }
}
