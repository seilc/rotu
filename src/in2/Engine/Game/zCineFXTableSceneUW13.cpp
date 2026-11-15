#include "xCommon.h"
#include "zCineFXTableSceneUW13.h"

#include "zCineFX.h"
#include "zCineFXShared.h"
#include "zFXBossBrain.h"

#define FX_HEAD_DUG_ID 0x191B9DF8 // xStrHash("fx_head_dug")
#define BOSS_DUG_DOME_ID 0x02081446 // xStrHash("Boss_DUG_dome")

namespace CineFXSceneUW13 {

namespace {

const cinfx_bossbrain_context braintext_dg_o_01 = { FX_HEAD_DUG_ID, 10, 0.1f, 0.2f, BOSS_DUG_DOME_ID };

} // namespace

static NCINEntry g_cinfx_UW13_01_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(200), "DG_O_01 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 0, 3 },
      { { { 0.0f, 1.2f, 0.0f } } },
      (void*)&braintext_dg_o_01 },
    { 0 }
};

static NCINEntry g_cinfx_UW13_02_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(200), "DG_O_02 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 2, 3 },
      { { { 0.0f, 1.2f, 0.0f } } },
      (void*)&braintext_dg_o_01 },
    { 0 }
};

static NCINEntry g_cinfx_UW13_03_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(200), "DG_O_03 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 0, 3 },
      { { { 0.0f, 1.2f, 0.0f } } },
      (void*)&braintext_dg_o_01 },
    { 0 }
};

static NCINEntry g_cinfx_UW13_05_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(400), "DG_O_05 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 2, 3 },
      { { { 0.0f, 1.2f, 0.0f } } },
      (void*)&braintext_dg_o_01 },
    { 0 }
};

static NCINEntry g_cinfx_UW13_07_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(400), "DG_O_07 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 0, 3 },
      { { { 0.0f, 1.2f, 0.0f } } },
      (void*)&braintext_dg_o_01 },
    { 0 }
};

static NCINEntry g_cinfx_UW13_08_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(200), "DG_O_08 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 2, 3 },
      { { { 0.0f, 1.2f, 0.0f } } },
      (void*)&braintext_dg_o_01 },
    { 0 }
};

static NCINEntry g_cinfx_UW13_09_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(200), "DG_O_09 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 2, 3 },
      { { { 0.0f, 1.2f, 0.0f } } },
      (void*)&braintext_dg_o_01 },
    { 0 }
};

static NCINEntry g_cinfx_UW13_10_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(200), "DG_O_10 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 0, 3 },
      { { { 0.0f, 1.2f, 0.0f } } },
      (void*)&braintext_dg_o_01 },
    { 0 }
};

static NCINEntry g_cinfx_UW13_11_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(300), "DG_O_11 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 2, 3 },
      { { { 0.0f, 1.2f, 0.0f } } },
      (void*)&braintext_dg_o_01 },
    { 0 }
};

static NCINEntry g_cinfx_UW13_12_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(200), "DG_O_12 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 0, 3 },
      { { { 0.0f, 1.2f, 0.0f } } },
      (void*)&braintext_dg_o_01 },
    { 0 }
};

} // namespace CineFXSceneUW13

void zCineFX_Register_SceneUW13()
{
    static NCINMap cutmap01[] = {
        { "07_DGO_010", CineFXSceneUW13::g_cinfx_UW13_01_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap02[] = {
        { "07_DGO_020", CineFXSceneUW13::g_cinfx_UW13_02_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap03[] = {
        { "07_DGO_030", CineFXSceneUW13::g_cinfx_UW13_03_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap05[] = {
        { "07_DGO_050", CineFXSceneUW13::g_cinfx_UW13_05_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap07[] = {
        { "07_DGO_070", CineFXSceneUW13::g_cinfx_UW13_07_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap08[] = {
        { "07_DGO_080", CineFXSceneUW13::g_cinfx_UW13_08_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap09[] = {
        { "07_DGO_090", CineFXSceneUW13::g_cinfx_UW13_09_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap10[] = {
        { "07_DGO_100", CineFXSceneUW13::g_cinfx_UW13_10_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap11[] = {
        { "07_DGO_110", CineFXSceneUW13::g_cinfx_UW13_11_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap12[] = {
        { "07_DGO_120", CineFXSceneUW13::g_cinfx_UW13_12_manager, 0 },
        { NULL, NULL, 0 }
    };

    zCineFXRegisterTable(cutmap01);
    zCineFXRegisterTable(cutmap02);
    zCineFXRegisterTable(cutmap03);
    zCineFXRegisterTable(cutmap05);
    zCineFXRegisterTable(cutmap07);
    zCineFXRegisterTable(cutmap08);
    zCineFXRegisterTable(cutmap09);
    zCineFXRegisterTable(cutmap10);
    zCineFXRegisterTable(cutmap11);
    zCineFXRegisterTable(cutmap12);
}
