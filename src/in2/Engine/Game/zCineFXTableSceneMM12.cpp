#include "xCommon.h"
#include "zCineFXTableSceneMM12.h"

#include "zCineFX.h"
#include "zCineFXShared.h"
#include "zFXBossBrain.h"

#define FX_HEAD_CRUST_ID 0x0A5FABE1 // xStrHash("fx_head_crust")

namespace CineFXSceneMM12 {

namespace {

const cinfx_bossbrain_context braintext_mm_e_02 = { FX_HEAD_CRUST_ID, 10, 0.1f, 0.3f };

} // namespace

static NCINEntry g_cinfx_MM12_02_manager[] = {
    { { 0x1001, CINFX_FRAMES(0), CINFX_FRAMES(200), "MM_E_02 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 3, 2 },
      { { { 0.0f, 0.65f, 0.0f } } },
      (void*)&braintext_mm_e_02 },
    { 0 }
};

static NCINEntry g_cinfx_MM12_03_manager[] = {
    { { 0x1001, CINFX_FRAMES(0), CINFX_FRAMES(200), "MM_E_03 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 1, 2 },
      { { { 0.0f, 0.65f, 0.0f } } },
      (void*)&braintext_mm_e_02 },
    { 0 }
};

static NCINEntry g_cinfx_MM12_07_manager[] = {
    { 0 }
};

} // namespace CineFXSceneMM12

void zCineFX_Register_SceneMM12()
{
    static NCINMap cutmap02[] = {
        { "05_MME_020", CineFXSceneMM12::g_cinfx_MM12_02_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap03[] = {
        { "05_MME_030", CineFXSceneMM12::g_cinfx_MM12_03_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap07[] = {
        { "05_MME_070", CineFXSceneMM12::g_cinfx_MM12_07_manager, 0 },
        { NULL, NULL, 0 }
    };

    zCineFXRegisterTable(cutmap02);
    zCineFXRegisterTable(cutmap03);
    zCineFXRegisterTable(cutmap07);
}
