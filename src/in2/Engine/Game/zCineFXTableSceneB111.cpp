#include "xCommon.h"
#include "zCineFXTableSceneB111.h"

#include "zCineFX.h"
#include "zCineFXShared.h"
#include "zFXBossBrain.h"

#define FX_HEAD_VERM_ID 0xDB87202C // xStrHash("fx_head_verm")
#define BOSS_VERM_DOME_ID 0x43D61802 // xStrHash("boss_verm_dome")

namespace CineFXSceneB111 {

namespace {

const cinfx_bossbrain_context braintext_b1_o_01 = { FX_HEAD_VERM_ID, 10, 0.1f, 0.32f, BOSS_VERM_DOME_ID, 0.35f };

} // namespace

static NCINEntry g_cinfx_B111_01_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(400), "B1_O_01 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 2, 4 },
      { { { 0.0f, 0.6f, 0.0f } } },
      (void*)&braintext_b1_o_01 },
    { 0 }
};

static NCINEntry g_cinfx_B111_02_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(400), "B1_O_02 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 0, 4 },
      { { { 0.0f, 0.6f, 0.0f } } },
      (void*)&braintext_b1_o_01 },
    { 0 }
};

static NCINEntry g_cinfx_B111_03_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(400), "B1_O_01 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 1, 4 },
      { { { 0.0f, 0.6f, 0.0f } } },
      (void*)&braintext_b1_o_01 },
    { 0 }
};

static NCINEntry g_cinfx_B111_06_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(400), "B1_O_02 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 0, 4 },
      { { { 0.0f, 0.6f, 0.0f } } },
      (void*)&braintext_b1_o_01 },
    { 0 }
};

static NCINEntry g_cinfx_B111_07_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(400), "B1_O_07 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 0, 4 },
      { { { 0.0f, 0.6f, 0.0f } } },
      (void*)&braintext_b1_o_01 },
    { 0 }
};

static NCINEntry g_cinfx_B111_08_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(400), "B1_O_08 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 0, 4 },
      { { { 0.0f, 0.6f, 0.0f } } },
      (void*)&braintext_b1_o_01 },
    { 0 }
};

static NCINEntry g_cinfx_B111_09_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(400), "B1_O_02 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 0, 4 },
      { { { 0.0f, 0.6f, 0.0f } } },
      (void*)&braintext_b1_o_01 },
    { 0 }
};

static NCINEntry g_cinfx_B111_10_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(400), "B1_O_02 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 0, 4 },
      { { { 0.0f, 0.6f, 0.0f } } },
      (void*)&braintext_b1_o_01 },
    { 0 }
};

static NCINEntry g_cinfx_B111_12_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(400), "B1_O_02 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 0, 4 },
      { { { 0.0f, 0.6f, 0.0f } } },
      (void*)&braintext_b1_o_01 },
    { 0 }
};

} // namespace CineFXSceneB111

void zCineFX_Register_SceneB111()
{
    static NCINMap cutmap01[] = {
        { "B1O_010", CineFXSceneB111::g_cinfx_B111_01_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap02[] = {
        { "B1O_020", CineFXSceneB111::g_cinfx_B111_02_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap03[] = {
        { "B1O_030", CineFXSceneB111::g_cinfx_B111_03_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap06[] = {
        { "B1O_060", CineFXSceneB111::g_cinfx_B111_06_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap07[] = {
        { "B1O_070", CineFXSceneB111::g_cinfx_B111_07_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap08[] = {
        { "B1O_080", CineFXSceneB111::g_cinfx_B111_08_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap09[] = {
        { "B1O_090", CineFXSceneB111::g_cinfx_B111_09_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap10[] = {
        { "B1O_100", CineFXSceneB111::g_cinfx_B111_10_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap12[] = {
        { "B1O_120", CineFXSceneB111::g_cinfx_B111_12_manager, 0 },
        { NULL, NULL, 0 }
    };

    zCineFXRegisterTable(cutmap01);
    zCineFXRegisterTable(cutmap02);
    zCineFXRegisterTable(cutmap03);
    zCineFXRegisterTable(cutmap06);
    zCineFXRegisterTable(cutmap07);
    zCineFXRegisterTable(cutmap08);
    zCineFXRegisterTable(cutmap09);
    zCineFXRegisterTable(cutmap10);
    zCineFXRegisterTable(cutmap12);
}
