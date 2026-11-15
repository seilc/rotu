#include "xCommon.h"
#include "zCineFXTableSceneRF11.h"

#include "zCineFX.h"
#include "zCineFXShared.h"
#include "zFXBossBrain.h"
#include "zHashes.h"

namespace CineFXSceneRF11 {

namespace {

const cinfx_bossbrain_context braintext_rf_o_01 = { HASH_fx_head_crust, 10, 0.1f, 0.3f };

} // namespace

static NCINEntry g_cinfx_RF11_01_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(200), "RF_O_01 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 0, 2 },
      { { { 0.0f, 0.65f, 0.0f } } },
      (void*)&braintext_rf_o_01 },
    { 0 }
};

static NCINEntry g_cinfx_RF11_03_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(200), "RF_O_03 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 1, 2 },
      { { { 0.0f, 0.65f, 0.0f } } },
      (void*)&braintext_rf_o_01 },
    { 0 }
};

static NCINEntry g_cinfx_RF11_04_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(200), "RF_O_04 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 1, 2 },
      { { { 0.0f, 0.65f, 0.0f } } },
      (void*)&braintext_rf_o_01 },
    { 0 }
};

static NCINEntry g_cinfx_RF11_06_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(400), "RF_O_06 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 1, 2 },
      { { { 0.0f, 0.65f, 0.0f } } },
      (void*)&braintext_rf_o_01 },
    { 0 }
};

} // namespace CineFXSceneRF11

void zCineFX_Register_SceneRF11()
{
    static NCINMap cutmap01[] = {
        { "RFO_010", CineFXSceneRF11::g_cinfx_RF11_01_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap03[] = {
        { "RFO_030", CineFXSceneRF11::g_cinfx_RF11_03_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap04[] = {
        { "RFO_040", CineFXSceneRF11::g_cinfx_RF11_04_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap06[] = {
        { "RFO_060", CineFXSceneRF11::g_cinfx_RF11_06_manager, 0 },
        { NULL, NULL, 0 }
    };

    zCineFXRegisterTable(cutmap01);
    zCineFXRegisterTable(cutmap03);
    zCineFXRegisterTable(cutmap04);
    zCineFXRegisterTable(cutmap06);
}
