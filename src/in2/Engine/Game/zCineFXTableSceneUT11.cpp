#include "xCommon.h"
#include "zCineFXTableSceneUT11.h"

#include "zCineFX.h"
#include "zCineFXShared.h"
#include "zForceFieldEffect.h"
#include "zFrozoneIce.h"
#include "xGlobals.h"
#include "xutil.h"

struct incrediball_params
{
    S32 ball_index;
    F32 scale;
    F32 height_offset;
    S32 num_models_inside;
    S32 models_inside_indices[3];
    U8 model_falls_out;
    F32 fall_out_time;
    S32 fall_out_index;
    F32 impact_amount_scale;
    F32 impact_amount_min;
    F32 impact_amount_max;
    F32 impact_radius;
    F32 impact_threshold;
    F32 ground_radius;
    F32 ground_amount;
    F32 roll_radius;
    F32 roll_scale;
    bool no_distortion;
    struct _runtime
    {
        xVec3 last_vel;
        xMat3x3 rot;
    } runtime;
};

namespace CineFXSceneUT11 {

namespace {

zForceFieldEffect* ffe;

} // namespace

static xMat4x3 zCinFX_Incrediball_position;

static void zCinFX_IB_Twk(const char*, NCINEntry*)
{
}

static void zCinFX_Incrediball_Update(const zCutsceneMgr*, NCINEntry* fxrec, S32 killit)
{
    incrediball_params* params = (incrediball_params*)fxrec->input.fxparm;

    if (killit) {
        fxrec->flg_stat |= 0x2;
        ffe->FreeForceField();
        ffe = NULL;
        return;
    }

    if (fxrec->flg_stat & 0x4) {
        ffe = zForceFieldEffect::GetForceField(0.0f, 1.0f, xVec3::m_Null, false);
        xFColor color = { 136.0f / 255.0f, 15.0f / 255.0f, 1.0f, 1.0f };
        ffe->SetShieldColor(color);
        ffe->SetShieldDistortion(!params->no_distortion);
        params->runtime.rot = g_I3;
        zCinFX_Incrediball_position.pos.x = -HUGE;
    }
}

static void zCinFX_Incrediball_AR(const zCutsceneMgr*, NCINEntry* fxrec, RpAtomic*, RwMatrix* animMat, U32, U32,
                                  U32& visFlags)
{
    incrediball_params* params = (incrediball_params*)fxrec->input.fxparm;

    visFlags = 0;

    xMat4x3 mat;
    mat = *(const xMat4x3*)animMat;
    xMat3x3MulScaleC(mat, params->scale);
    mat.pos.y += params->height_offset;

    if (zCinFX_Incrediball_position.pos.x != -HUGE) {
        xVec3 delta = mat.pos;
        delta -= zCinFX_Incrediball_position.pos;

        F32 angle = -delta.length() / params->roll_radius;

        xVec3 axis;
        axis.cross_ypos(delta);
        axis.safe_normalize(g_Z3);

        xMat3x3 rot;
        xMat3x3Rot(&rot, &axis, params->roll_scale * angle);
        xMat3x3Mul(&params->runtime.rot, &params->runtime.rot, &rot);
        xMat3x3Normalize(&params->runtime.rot, &params->runtime.rot);

        (xMat3x3&)mat = params->runtime.rot;
        xMat3x3MulScaleC(mat, params->scale);
        ffe->Move(mat);

        xVec3 vel = delta;
        vel /= xglobals->update_dt;

        xVec3 accel = vel;
        accel -= params->runtime.last_vel;
        params->runtime.last_vel = vel;

        F32 len = accel.length();
        if (len > params->impact_threshold) {
            F32 amount = params->impact_amount_scale * len;
            amount = CLAMP(amount, params->impact_amount_min, params->impact_amount_max);
            xVec3 loc = { mat.pos.x - accel.x, mat.pos.y - accel.y, mat.pos.z - accel.z };
            ffe->Impact(loc, params->impact_radius, -amount, false);
        }
    } else {
        ffe->Move(mat);
    }

    xVec3 loc = { mat.pos.x, mat.pos.y - 1.3f, mat.pos.z };
    ffe->Impact(loc, params->ground_radius, -params->ground_amount, true);

    zCinFX_Incrediball_position = mat;
}

static void zCinFX_Incrediball_Render(const zCutsceneMgr*, NCINEntry*)
{
    ffe->Update(xglobals->update_dt);
}

namespace {

const zFrozoneIceRecordEntry record_ut_o_01[] = {
    {  2.243903f, { -103.26048f,  27.49090f, -358.95963f } },
    {  2.263703f, { -102.50144f,  27.48922f, -358.95654f } },
    {  2.284103f, { -101.60234f,  27.48620f, -358.96463f } },
    {  2.303903f, { -100.72166f,  27.48775f, -358.97806f } },
    {  2.323703f, { -99.89053f,  27.48364f, -358.99896f } },
    {  2.344103f, { -99.16726f,  27.47722f, -359.02817f } },
    {  2.374103f, { -98.29145f,  27.46225f, -359.09540f } },
    {  2.404103f, { -97.45902f,  27.44163f, -359.21390f } },
    {  2.434103f, { -96.63836f,  27.41412f, -359.37503f } },
    {  2.464103f, { -95.83205f,  27.37625f, -359.56281f } },
    {  2.494102f, { -95.01585f,  27.33378f, -359.76492f } },
    {  2.524102f, { -94.16130f,  27.28331f, -359.96194f } },
    {  2.554102f, { -93.32634f,  27.21203f, -360.14020f } },
    {  2.584102f, { -92.51991f,  27.12263f, -360.28580f } },
    {  2.614102f, { -91.74169f,  27.01520f, -360.37366f } },
    {  2.644102f, { -90.99068f,  26.88994f, -360.37778f } },
    {  2.684302f, { -90.03416f,  26.70194f, -360.22504f } },
    {  2.724502f, { -89.10478f,  26.49349f, -359.92596f } },
    {  2.754502f, { -88.42854f,  26.31933f, -359.64612f } },
    {  2.794702f, { -87.65700f,  26.09325f, -359.29297f } },
    {  2.844502f, { -86.83467f,  25.84968f, -359.04413f } },
    {  2.894302f, { -85.99142f,  25.62816f, -358.95377f } },
    {  2.934502f, { -85.22666f,  25.44649f, -358.88074f } },
    {  2.974702f, { -84.40515f,  25.23963f, -358.72903f } },
    {  3.014902f, { -83.58424f,  25.00945f, -358.51172f } },
    {  3.054502f, { -82.74113f,  24.69638f, -358.28220f } },
    {  3.084502f, { -82.03951f,  24.39109f, -358.07565f } },
    {  3.114502f, { -81.31305f,  24.07954f, -357.82452f } },
    {  3.145102f, { -80.56997f,  23.78433f, -357.49435f } },
    {  3.175102f, { -79.87647f,  23.54603f, -357.07199f } },
    {  3.205102f, { -79.25985f,  23.36849f, -356.53467f } },
    {  3.235102f, { -78.73683f,  23.21778f, -355.80661f } },
    {  3.254902f, { -78.43773f,  23.08572f, -355.19025f } },
    {  3.274702f, { -78.16856f,  22.95557f, -354.53729f } },
    {  3.295102f, { -77.92947f,  22.81834f, -353.82330f } },
    {  3.314902f, { -77.74119f,  22.67599f, -353.12213f } },
    {  3.334702f, { -77.58308f,  22.54486f, -352.42032f } },
    {  3.355102f, { -77.45499f,  22.41110f, -351.73404f } },
    {  3.385102f, { -77.33807f,  22.23812f, -350.81137f } },
    {  3.415102f, { -77.32469f,  22.07961f, -350.03293f } },
    {  3.455302f, { -77.35680f,  21.89912f, -349.09918f } },
    {  3.485302f, { -77.39819f,  21.72942f, -348.31558f } },
    {  3.515302f, { -77.48313f,  21.53068f, -347.44080f } },
    {  3.545302f, { -77.58498f,  21.33472f, -346.55508f } },
    {  3.575302f, { -77.66416f,  21.15821f, -345.65723f } },
    {  3.605302f, { -77.77998f,  21.00406f, -344.69324f } },
    {  3.635302f, { -78.01670f,  20.87476f, -343.73248f } },
    {  3.675502f, { -78.39939f,  20.77836f, -342.93842f } },
    {  3.715101f, { -79.02752f,  20.77241f, -342.24600f } },
    { -1.0f },
    {  4.556305f, { -85.10830f,  17.60688f, -330.62939f } },
    {  4.586305f, { -84.98238f,  16.95056f, -330.01706f } },
    {  4.626505f, { -84.37555f,  16.56231f, -329.36505f } },
    {  4.656506f, { -83.79084f,  16.31864f, -328.83389f } },
    {  4.686506f, { -83.20778f,  16.13090f, -328.24329f } },
    {  4.716506f, { -82.64896f,  16.02108f, -327.59961f } },
    {  4.746506f, { -82.07661f,  15.96310f, -326.90649f } },
    {  4.776506f, { -81.46224f,  15.91074f, -326.18576f } },
    {  4.806507f, { -80.82324f,  15.84264f, -325.45096f } },
    {  4.826307f, { -80.39228f,  15.77663f, -324.95944f } },
    {  4.846707f, { -79.93600f,  15.65814f, -324.42975f } },
    {  4.866507f, { -79.48785f,  15.51705f, -323.90396f } },
    {  4.886307f, { -79.04129f,  15.34034f, -323.39316f } },
    {  4.906707f, { -78.58739f,  15.16435f, -322.89276f } },
    {  4.936707f, { -77.94206f,  14.93356f, -322.25650f } },
    {  4.966708f, { -77.32775f,  14.78334f, -321.79141f } },
    {  5.006908f, { -76.53079f,  14.63331f, -321.30878f } },
    {  5.046508f, { -75.82859f,  14.46905f, -320.85577f } },
    {  5.086709f, { -75.17630f,  14.23516f, -320.35565f } },
    {  5.387311f, { -74.62664f,  13.96809f, -319.92896f } },
};

const cinfx_frobridge_context frotext_ut_o_01 = { 0, record_ut_o_01, ARRAY_LENGTH(record_ut_o_01) };

incrediball_params balltext_ut_o_10 = {
    0, 1.3f, -0.2f, 2, { 2, 4 }, 0, 0.0f, 0, 0.05f, 0.1f, 0.4f, 0.2f, 10.0f, 0.2f, 0.2f, 1.015f, 0.2f, true
};

incrediball_params balltext_ut_o_14 = {
    0, 1.3f, -0.2f, 2, { 2, 4 }, 0, 0.0f, 0, 0.05f, 0.1f, 0.4f, 0.2f, 10.0f, 0.2f, 0.2f, 1.015f, 0.2f, false
};

} // namespace

static NCINEntry g_cinfx_UT11_01_manager[] = {
    { { 0x1000, CINFX_FRAMES(60), CINFX_FRAMES(270), "UTO_01 Ice Glide" },
      { cinfx_frobridge_update, cinfx_frobridge_animate, cinfx_frobridge_render, cinfx_frobridge_add_tweaks },
      { 0, 51, 47 },
      { 0 },
      (void*)&frotext_ut_o_01 },
    { 0 }
};

static NCINEntry g_cinfx_UT11_10_manager[] = {
    { { 0x1001, CINFX_FRAMES(0), CINFX_FRAMES(400), "UTO_10 Incrediball" },
      { zCinFX_Incrediball_Update, zCinFX_Incrediball_AR, zCinFX_Incrediball_Render, zCinFX_IB_Twk },
      { 10 },
      { 0 },
      &balltext_ut_o_10 },
    { 0 }
};

static NCINEntry g_cinfx_UT11_14_manager[] = {
    { { 0x1001, CINFX_FRAMES(0), CINFX_FRAMES(400), "UTO_10 Incrediball" },
      { zCinFX_Incrediball_Update, zCinFX_Incrediball_AR, zCinFX_Incrediball_Render, zCinFX_IB_Twk },
      { 7 },
      { 0 },
      &balltext_ut_o_14 },
    { 0 }
};

} // namespace CineFXSceneUT11

void zCineFX_Register_SceneUT11()
{
    static NCINMap cutmap01[] = {
        { "01_UTO_010", CineFXSceneUT11::g_cinfx_UT11_01_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap10[] = {
        { "01_UTO_100", CineFXSceneUT11::g_cinfx_UT11_10_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap14[] = {
        { "01_UTO_140", CineFXSceneUT11::g_cinfx_UT11_14_manager, 0 },
        { NULL, NULL, 0 }
    };

    zCineFXRegisterTable(cutmap01);
    zCineFXRegisterTable(cutmap10);
    zCineFXRegisterTable(cutmap14);
}
