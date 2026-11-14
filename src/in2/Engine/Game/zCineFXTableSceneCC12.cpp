#include "xCommon.h"
#include "zCineFXTableSceneCC12.h"

#include "zCineFX.h"
#include "zCineFXShared.h"
#include "zFrozoneBlast.h"

namespace CineFXSceneCC12 {

namespace {

const cinfx_spray_context spraytext_cc_e_12 = { 10.0f, false };

const cinfx_spray_context spraytext_cc_e_13 = { 7.0f, true };

} // namespace

static NCINEntry g_cinfx_CC12_12_manager[] = {
    { { 0x1001, CINFX_FRAMES(6), CINFX_FRAMES(21), "CC_E_12 Freeze Blast (Left)" },
      { zFrozoneBlast::cinfxUpdate, zFrozoneBlast::cinfxAnimate, zFrozoneBlast::cinfxRender,
        zFrozoneBlast::cinfxAddTweaks },
      { 0, 32 },
      { { { 0.1f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.8f } } },
      (void*)&spraytext_cc_e_12 },
    { { 0x1001, CINFX_FRAMES(12), CINFX_FRAMES(21), "CC_E_12 Freeze Blast (Right)" },
      { zFrozoneBlast::cinfxUpdate, zFrozoneBlast::cinfxAnimate, zFrozoneBlast::cinfxRender,
        zFrozoneBlast::cinfxAddTweaks },
      { 0, 13 },
      { { { -0.1f, 0.0f, 0.0f }, { 0.0f, 0.2f, -1.0f } } },
      (void*)&spraytext_cc_e_12 },
    { 0 }
};

static NCINEntry g_cinfx_CC12_13_manager[] = {
    { { 0x1001, CINFX_FRAMES(0), CINFX_FRAMES(25), "CC_E_13 Freeze Blast" },
      { zFrozoneBlast::cinfxUpdate, zFrozoneBlast::cinfxAnimate, zFrozoneBlast::cinfxRender,
        zFrozoneBlast::cinfxAddTweaks },
      { 0, 0 },
      { { { -8.9f, 0.6f, 1.7f }, { -1.3f, PI / 2, 0.0f } } },
      (void*)&spraytext_cc_e_13 },
    { 0 }
};

} // namespace CineFXSceneCC12

void zCineFX_Register_SceneCC12()
{
    static NCINMap cutmap12[] = {
        { "08_CCE_120", CineFXSceneCC12::g_cinfx_CC12_12_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap13[] = {
        { "08_CCE_130", CineFXSceneCC12::g_cinfx_CC12_13_manager, 0 },
        { NULL, NULL, 0 }
    };

    zCineFXRegisterTable(cutmap12);
    zCineFXRegisterTable(cutmap13);
}
