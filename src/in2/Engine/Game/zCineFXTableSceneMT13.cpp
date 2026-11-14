#include "xCommon.h"
#include "zCineFXTableSceneMT13.h"

#include "zCineFX.h"
#include "zCineFXShared.h"
#include "zNPCBomber.h"

namespace CineFXSceneMT13 {

static NCINEntry g_cinfx_MT13_01_manager[] = {
    { { 0x1000, CINFX_FRAMES(75), CINFX_FRAMES(135), "MT_O_Excavator Drill Trails 1" },
      { zNPC::zNPCBomber::cinfxDrillTrailUpdate, zNPC::zNPCBomber::cinfxDrillTrailAnimate,
        zNPC::zNPCBomber::cinfxDrillTrailRender, zNPC::zNPCBomber::cinfxDrillTrailAddTweaks },
      { 0, 1 },
      { { { 0.0f, 0.15f, 0.0f } } } },
    { { 0x1000, CINFX_FRAMES(230), CINFX_FRAMES(300), "MT_O_Excavator Drill Trails 2" },
      { zNPC::zNPCBomber::cinfxDrillTrailUpdate, zNPC::zNPCBomber::cinfxDrillTrailAnimate,
        zNPC::zNPCBomber::cinfxDrillTrailRender, zNPC::zNPCBomber::cinfxDrillTrailAddTweaks },
      { 0, 1 },
      { { { 0.0f, 0.15f, 0.0f } } } },
    { { 0x2000, CINFX_FRAMES(65), CINFX_FRAMES(175), "MT_O_Excavator Shield" },
      { zNPC::zNPCBomber::cinfxShieldUpdate, zNPC::zNPCBomber::cinfxShieldAnimate,
        zNPC::zNPCBomber::cinfxShieldRender, zNPC::zNPCBomber::cinfxShieldAddTweaks },
      { 0, 1 },
      { { { 0.0f, 0.2f, 0.0f } } } },
    { { 0x3000, CINFX_FRAMES(180), CINFX_FRAMES(215), "MT_O_Excavator Laser Blasts" },
      { zNPC::zNPCBomber::cinfxLaserBlastsUpdate, zNPC::zNPCBomber::cinfxLaserBlastsAnimate,
        zNPC::zNPCBomber::cinfxLaserBlastsRender, zNPC::zNPCBomber::cinfxLaserBlastsAddTweaks },
      { 0, 0 } },
    { { 0x6000, CINFX_FRAMES(135), CINFX_FRAMES(150), "MT_O_Excavator Drill Clods 1" },
      { zNPC::zNPCBomber::cinfxDrillClodsUpdate, zNPC::zNPCBomber::cinfxDrillClodsAnimate,
        zNPC::zNPCBomber::cinfxDrillClodsRender, zNPC::zNPCBomber::cinfxDrillClodsAddTweaks },
      { 0, 1 } },
    { { 0x6000, CINFX_FRAMES(215), CINFX_FRAMES(230), "MT_O_Excavator Drill Clods 2" },
      { zNPC::zNPCBomber::cinfxDrillClodsUpdate, zNPC::zNPCBomber::cinfxDrillClodsAnimate,
        zNPC::zNPCBomber::cinfxDrillClodsRender, zNPC::zNPCBomber::cinfxDrillClodsAddTweaks },
      { 0, 1 } },
    { { 0x4000, CINFX_FRAMES(230), CINFX_FRAMES(300), "MT_O_Excavator Smoke" },
      { zNPC::zNPCBomber::cinfxSmokeUpdate, zNPC::zNPCBomber::cinfxSmokeAnimate,
        zNPC::zNPCBomber::cinfxSmokeRender, zNPC::zNPCBomber::cinfxSmokeAddTweaks },
      { 0, 0 } },
    { { 0x5000, CINFX_FRAMES(0), CINFX_FRAMES(300), "MT_O_Excavator Light" },
      { zNPC::zNPCBomber::cinfxLightUpdate, zNPC::zNPCBomber::cinfxLightAnimate,
        zNPC::zNPCBomber::cinfxLightRender, zNPC::zNPCBomber::cinfxLightAddTweaks },
      { 0, 0 } },
    { { 0x7000, CINFX_FRAMES(135), CINFX_FRAMES(145), "MT_O_Excavator crater decal" },
      { zNPC::zNPCBomber::cinfxCraterDecalUpdate, zNPC::zNPCBomber::cinfxCraterDecalAnimate,
        zNPC::zNPCBomber::cinfxCraterDecalRender, zNPC::zNPCBomber::cinfxCraterDecalAddTweaks },
      { 0, 0 } },
    { 0 }
};

} // namespace CineFXSceneMT13

void zCineFX_Register_SceneMT13()
{
    static NCINMap cutmap01[] = {
        { "MT_O_Excavator", CineFXSceneMT13::g_cinfx_MT13_01_manager, 0 },
        { NULL, NULL, 0 }
    };

    zCineFXRegisterTable(cutmap01);
}
