#include "xCommon.h"
#include "zCineFXTableSceneB211.h"

#include "zCineFX.h"
#include "zCineFXShared.h"
#include "zFXBossBrain.h"

#define FX_HEAD_CRUST_ID 0x0A5FABE1 // xStrHash("fx_head_crust")
#define BOSS_CRUST_DOME_ID 0xEFABF071 // xStrHash("boss_crust_dome")

namespace CineFXSceneB211 {

namespace {

const cinfx_bossbrain_context braintext_b2_o_01 = { FX_HEAD_CRUST_ID, 10, 0.1f, 0.3f, BOSS_CRUST_DOME_ID };

} // namespace

static NCINEntry g_cinfx_B211_01_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(400), "B2_O_01 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 1, 24 },
      { { { 0.0f, 0.65f, 0.0f } } },
      (void*)&braintext_b2_o_01 },
    { 0 }
};

static NCINEntry g_cinfx_B211_02_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(400), "B2_O_02 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 1, 24 },
      { { { 0.0f, 0.65f, 0.0f } } },
      (void*)&braintext_b2_o_01 },
    { 0 }
};

static NCINEntry g_cinfx_B211_03_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(400), "B2_O_03 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 1, 24 },
      { { { 0.0f, 0.65f, 0.0f } } },
      (void*)&braintext_b2_o_01 },
    { 0 }
};

static NCINEntry g_cinfx_B211_06_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(400), "B2_O_06 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 1, 24 },
      { { { 0.0f, 0.65f, 0.0f } } },
      (void*)&braintext_b2_o_01 },
    { 0 }
};

static NCINEntry g_cinfx_B211_09_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(400), "B2_O_09 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 1, 24 },
      { { { 0.0f, 0.65f, 0.0f } } },
      (void*)&braintext_b2_o_01 },
    { 0 }
};

static NCINEntry g_cinfx_B211_10_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(400), "B2_O_10 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 1, 24 },
      { { { 0.0f, 0.65f, 0.0f } } },
      (void*)&braintext_b2_o_01 },
    { 0 }
};

} // namespace CineFXSceneB211

void zCineFX_Register_SceneB211()
{
    static NCINMap cutmap01[] = {
        { "B2O_010", CineFXSceneB211::g_cinfx_B211_01_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap02[] = {
        { "B2O_020", CineFXSceneB211::g_cinfx_B211_02_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap03[] = {
        { "B2O_030", CineFXSceneB211::g_cinfx_B211_03_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap06[] = {
        { "B2O_060", CineFXSceneB211::g_cinfx_B211_06_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap09[] = {
        { "B2O_090", CineFXSceneB211::g_cinfx_B211_09_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap10[] = {
        { "B2O_100", CineFXSceneB211::g_cinfx_B211_10_manager, 0 },
        { NULL, NULL, 0 }
    };

    zCineFXRegisterTable(cutmap01);
    zCineFXRegisterTable(cutmap02);
    zCineFXRegisterTable(cutmap03);
    zCineFXRegisterTable(cutmap06);
    zCineFXRegisterTable(cutmap09);
    zCineFXRegisterTable(cutmap10);
}
