#include "xCommon.h"
#include "zCineFXTableSceneUT14.h"

#include "zCineFX.h"
#include "zCineFXShared.h"
#include "zNPCEnforcer.h"

namespace CineFXSceneUT14 {

static NCINEntry g_cinfx_UT14_01_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(500), "enforcer_opening Enforcer Smoke" },
      { zNPC::zNPCEnforcer::cinfxSmokeUpdate, zNPC::zNPCEnforcer::cinfxSmokeAnimate,
        zNPC::zNPCEnforcer::cinfxSmokeRender, zNPC::zNPCEnforcer::cinfxSmokeAddTweaks },
      { 1, 10 },
      { { { -1.15f, 0.55f, 0.0f }, { -2.0f, 0.0f, 0.0f } } } },
    { { 0x2000, CINFX_FRAMES(75), CINFX_FRAMES(500), "enforcer_opening Enforcer Drilltrail" },
      { zNPC::zNPCEnforcer::cinfxDrillTrailUpdate, zNPC::zNPCEnforcer::cinfxDrillTrailAnimate,
        zNPC::zNPCEnforcer::cinfxDrillTrailRender, zNPC::zNPCEnforcer::cinfxSmokeAddTweaks },
      { 1, 0 },
      { { { 0.0f, 0.45f, 0.0f } } } },
    { 0 }
};

} // namespace CineFXSceneUT14

void zCineFX_Register_SceneUT14()
{
    static NCINMap cutmap01[] = {
        { "enforcer_opening", CineFXSceneUT14::g_cinfx_UT14_01_manager, 0 },
        { NULL, NULL, 0 }
    };

    zCineFXRegisterTable(cutmap01);
}
