#include "xCommon.h"
#include "zCineFXTableSceneUW12.h"

#include "zCineFX.h"
#include "zCineFXShared.h"
#include "zFXBossBrain.h"

#define FX_HEAD_DUG_ID 0x191B9DF8 // xStrHash("fx_head_dug")
#define BOSS_DUG_DOME_ID 0x02081446 // xStrHash("Boss_DUG_dome")

namespace CineFXSceneUW12 {

namespace {

const cinfx_bossbrain_context braintext_uw_e_01 = { FX_HEAD_DUG_ID, 10, 0.1f, 0.2f, BOSS_DUG_DOME_ID };

} // namespace

static NCINEntry g_cinfx_UW12_01_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(400), "UW_E_01 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 2, 3 },
      { { { 0.0f, 1.2f, 0.0f } } },
      (void*)&braintext_uw_e_01 },
    { 0 }
};

static NCINEntry g_cinfx_UW12_02_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(400), "UW_E_02 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 0, 3 },
      { { { 0.0f, 1.2f, 0.0f } } },
      (void*)&braintext_uw_e_01 },
    { 0 }
};

static NCINEntry g_cinfx_UW12_03_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(400), "UW_E_03 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 1, 3 },
      { { { 0.0f, 1.2f, 0.0f } } },
      (void*)&braintext_uw_e_01 },
    { 0 }
};

static NCINEntry g_cinfx_UW12_04_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(400), "UW_E_04 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 0, 3 },
      { { { 0.0f, 1.2f, 0.0f } } },
      (void*)&braintext_uw_e_01 },
    { 0 }
};

static NCINEntry g_cinfx_UW12_06_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(400), "UW_E_06 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 0, 3 },
      { { { 0.0f, 1.2f, 0.0f } } },
      (void*)&braintext_uw_e_01 },
    { 0 }
};

static NCINEntry g_cinfx_UW12_07_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(400), "UW_E_07 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 0, 3 },
      { { { 0.0f, 1.2f, 0.0f } } },
      (void*)&braintext_uw_e_01 },
    { 0 }
};

static NCINEntry g_cinfx_UW12_08_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(400), "UW_E_08 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 0, 3 },
      { { { 0.0f, 1.2f, 0.0f } } },
      (void*)&braintext_uw_e_01 },
    { 0 }
};

} // namespace CineFXSceneUW12

void zCineFX_Register_SceneUW12()
{
    static NCINMap cutmap01[] = {
        { "07_UWE_010", CineFXSceneUW12::g_cinfx_UW12_01_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap02[] = {
        { "07_UWE_020", CineFXSceneUW12::g_cinfx_UW12_02_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap03[] = {
        { "07_UWE_030", CineFXSceneUW12::g_cinfx_UW12_03_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap04[] = {
        { "07_UWE_040", CineFXSceneUW12::g_cinfx_UW12_04_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap06[] = {
        { "07_UWE_060", CineFXSceneUW12::g_cinfx_UW12_06_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap07[] = {
        { "07_UWE_070", CineFXSceneUW12::g_cinfx_UW12_07_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap08[] = {
        { "07_UWE_080", CineFXSceneUW12::g_cinfx_UW12_08_manager, 0 },
        { NULL, NULL, 0 }
    };

    zCineFXRegisterTable(cutmap01);
    zCineFXRegisterTable(cutmap02);
    zCineFXRegisterTable(cutmap03);
    zCineFXRegisterTable(cutmap04);
    zCineFXRegisterTable(cutmap06);
    zCineFXRegisterTable(cutmap07);
    zCineFXRegisterTable(cutmap08);
}
