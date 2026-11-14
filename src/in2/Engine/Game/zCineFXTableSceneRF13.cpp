#include "xCommon.h"
#include "zCineFXTableSceneRF13.h"

#include "zCineFX.h"
#include "zCineFXShared.h"
#include "zNPCRobotTank.h"

namespace CineFXSceneRF13 {

static NCINEntry g_cinfx_RF13_01_manager[] = {
    { { 0x1000, CINFX_FRAMES(270), CINFX_FRAMES(300), "tank_opening Tank Slam" },
      { zNPC::zNPCRobotTank::cinfxTankSlamUpdate, zNPC::zNPCRobotTank::cinfxTankSlamAnimate,
        zNPC::zNPCRobotTank::cinfxTankSlamRender, zNPC::zNPCRobotTank::cinfxTankSlamAddTweaks },
      { 0, 21 } },
    { 0 }
};

} // namespace CineFXSceneRF13

void zCineFX_Register_SceneRF13()
{
    static NCINMap cutmap01[] = {
        { "tank_opening", CineFXSceneRF13::g_cinfx_RF13_01_manager, 0 },
        { NULL, NULL, 0 }
    };

    zCineFXRegisterTable(cutmap01);
}
