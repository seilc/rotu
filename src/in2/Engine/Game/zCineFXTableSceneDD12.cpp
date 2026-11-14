#include "xCommon.h"
#include "zCineFXTableSceneDD12.h"

#include "zCineFX.h"
#include "zCineFXShared.h"
#include "zFrozoneIce.h"
#include "xutil.h"

namespace CineFXSceneDD12 {

namespace {

const zFrozoneIceRecordEntry record_dd_e_01[] = {
    {  0.000000f, { -188.39157f, -18.72166f, -488.72180f } },
    {  0.174000f, { -188.97139f, -18.73421f, -489.51859f } },
    {  0.252000f, { -189.68820f, -18.63030f, -490.09579f } },
    {  0.319200f, { -190.32462f, -18.54804f, -490.62692f } },
    {  0.395200f, { -190.94235f, -18.51595f, -491.26346f } },
    {  0.486400f, { -191.44984f, -18.51468f, -492.01736f } },
    {  0.581400f, { -191.63588f, -18.47857f, -492.71561f } },
    {  0.658400f, { -191.29190f, -18.50651f, -493.44952f } },
    {  0.714900f, { -190.63773f, -18.80014f, -493.97458f } },
    {  0.810400f, { -189.83652f, -19.29979f, -494.26859f } },
    {  0.904900f, { -189.16714f, -19.47275f, -494.69040f } },
    {  0.981400f, { -188.40685f, -19.47676f, -495.19824f } },
    {  1.000900f, { -187.77925f, -19.33456f, -496.06326f } },
    {  1.157900f, { -187.75653f, -19.32174f, -497.01566f } },
    {  1.260400f, { -188.24683f, -19.52484f, -497.78815f } },
    {  1.339900f, { -188.82939f, -19.73376f, -498.28754f } },
    {  1.419900f, { -189.56644f, -19.94392f, -498.78055f } },
    {  1.479900f, { -190.19081f, -20.07349f, -499.17535f } },
    {  1.539900f, { -190.84422f, -20.17833f, -499.60110f } },
    {  1.599900f, { -191.52649f, -20.25000f, -500.06720f } },
    {  1.659900f, { -192.20253f, -20.28475f, -500.55972f } },
};

const cinfx_frobridge_context frotext_dd_e_01 = { 0, record_dd_e_01, ARRAY_LENGTH(record_dd_e_01) };

const zFrozoneIceRecordEntry record_dd_e_02[] = {
    {  0.027500f, { -197.52612f, -21.18712f, -503.84940f } },
    {  0.195500f, { -198.29408f, -21.17373f, -504.27487f } },
    {  0.252000f, { -199.00768f, -21.16845f, -504.64749f } },
    {  0.309500f, { -199.84317f, -21.16930f, -505.07431f } },
    {  0.366500f, { -200.69571f, -21.17572f, -505.50464f } },
    {  0.423500f, { -201.47926f, -21.19596f, -505.88788f } },
    {  0.518500f, { -202.36688f, -21.25757f, -506.30820f } },
    {  0.826000f, { -203.28259f, -21.41702f, -506.50702f } },
    {  1.092500f, { -204.22868f, -21.39297f, -506.63571f } },
    {  1.190500f, { -204.36368f, -21.41171f, -506.66592f } },
};

const cinfx_frobridge_context frotext_dd_e_02 = { 0, record_dd_e_02, ARRAY_LENGTH(record_dd_e_02) };

} // namespace

static NCINEntry g_cinfx_DD12_01_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(300), "DD_E_01 Ice Glide" },
      { cinfx_frobridge_update, cinfx_frobridge_animate, cinfx_frobridge_render, cinfx_frobridge_add_tweaks },
      { 1, 51, 55 },
      { 0 },
      (void*)&frotext_dd_e_01 },
    { 0 }
};

static NCINEntry g_cinfx_DD12_02_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(300), "DD_E_02 Ice Glide" },
      { cinfx_frobridge_update, cinfx_frobridge_animate, cinfx_frobridge_render, cinfx_frobridge_add_tweaks },
      { 0, 51, 55 },
      { 0 },
      (void*)&frotext_dd_e_02 },
    { 0 }
};

} // namespace CineFXSceneDD12

void zCineFX_Register_SceneDD12()
{
    static NCINMap cutmap01[] = {
        { "03_DDE_010", CineFXSceneDD12::g_cinfx_DD12_01_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap02[] = {
        { "03_DDE_020", CineFXSceneDD12::g_cinfx_DD12_02_manager, 0 },
        { NULL, NULL, 0 }
    };

    zCineFXRegisterTable(cutmap01);
    zCineFXRegisterTable(cutmap02);
}
