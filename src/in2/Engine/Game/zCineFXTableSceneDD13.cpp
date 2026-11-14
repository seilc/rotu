#include "xCommon.h"
#include "zCineFXTableSceneDD13.h"

#include "zCineFX.h"
#include "zCineFXShared.h"
#include "zNPCDriller.h"

namespace CineFXSceneDD13 {

static NCINEntry g_cinfx_DD13_01_manager[] = {
    { { 0x1000, CINFX_FRAMES(30), CINFX_FRAMES(40), "driller_opening Tunnel Up" },
      { zNPC::zNPCDriller::cinfxDrillTunnelUpUpdate, zNPC::zNPCDriller::cinfxDrillTunnelUpAnimate,
        zNPC::zNPCDriller::cinfxDrillTunnelUpRender, zNPC::zNPCDriller::cinfxDrillTunnelUpAddTweaks },
      { 0, 0 } },
    { { 0x2000, CINFX_FRAMES(185), CINFX_FRAMES(215), "driller_opening Tunnel Down" },
      { zNPC::zNPCDriller::cinfxDrillTunnelDownUpdate, zNPC::zNPCDriller::cinfxDrillTunnelDownAnimate,
        zNPC::zNPCDriller::cinfxDrillTunnelDownRender, zNPC::zNPCDriller::cinfxDrillTunnelDownAddTweaks },
      { 0, 2 } },
    { { 0x3000, CINFX_FRAMES(215), CINFX_FRAMES(220), "driller_opening Tunnel Plume" },
      { zNPC::zNPCDriller::cinfxDrillTunnelPlumeUpdate, zNPC::zNPCDriller::cinfxDrillTunnelPlumeAnimate,
        zNPC::zNPCDriller::cinfxDrillTunnelPlumeRender, zNPC::zNPCDriller::cinfxDrillTunnelPlumeAddTweaks },
      { 0, 8 } },
    { { 0x4000, CINFX_FRAMES(70), CINFX_FRAMES(75), "driller_opening Driller Land" },
      { zNPC::zNPCDriller::cinfxDrillerLandUpdate, zNPC::zNPCDriller::cinfxDrillerLandAnimate,
        zNPC::zNPCDriller::cinfxDrillerLandRender, zNPC::zNPCDriller::cinfxDrillerLandAddTweaks },
      { 0, 2 } },
    { 0 }
};

} // namespace CineFXSceneDD13

void zCineFX_Register_SceneDD13()
{
    static NCINMap cutmap01[] = {
        { "driller_opening", CineFXSceneDD13::g_cinfx_DD13_01_manager, 0 },
        { NULL, NULL, 0 }
    };

    zCineFXRegisterTable(cutmap01);
}
