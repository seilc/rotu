#include "xCommon.h"
#include "zCineFXTableSceneUT15.h"

#include "zCineFX.h"
#include "zCineFXShared.h"
#include "zNPCGroundShooter.h"

namespace CineFXSceneUT15 {

static NCINEntry g_cinfx_UT15_01_manager[] = {
    { { 0x1000, CINFX_FRAMES(48), CINFX_FRAMES(55), "UT15 firing F1" },
      { zNPC::zNPCGroundShooter::cinfxUpdateUT15, zNPC::zNPCGroundShooter::cinfxAnimateUT15,
        zNPC::zNPCGroundShooter::cinfxRenderUT15, NULL },
      { 3, 19 } },
    { { 0x1000, CINFX_FRAMES(66.1f), CINFX_FRAMES(70), "UT15 firing F2" },
      { zNPC::zNPCGroundShooter::cinfxUpdateUT15, zNPC::zNPCGroundShooter::cinfxAnimateUT15,
        zNPC::zNPCGroundShooter::cinfxRenderUT15, NULL },
      { 1, 19 } },
    { { 0x1000, CINFX_FRAMES(81.5f), CINFX_FRAMES(85), "UT15 firing F3" },
      { zNPC::zNPCGroundShooter::cinfxUpdateUT15, zNPC::zNPCGroundShooter::cinfxAnimateUT15,
        zNPC::zNPCGroundShooter::cinfxRenderUT15, NULL },
      { 2, 19 } },
    { { 0x1000, CINFX_FRAMES(111.8f), CINFX_FRAMES(120), "UT15 firing F4a" },
      { zNPC::zNPCGroundShooter::cinfxUpdateUT15, zNPC::zNPCGroundShooter::cinfxAnimateUT15,
        zNPC::zNPCGroundShooter::cinfxRenderUT15, NULL },
      { 0, 19 } },
    { { 0x1000, CINFX_FRAMES(143), CINFX_FRAMES(150), "UT15 firing F4b" },
      { zNPC::zNPCGroundShooter::cinfxUpdateUT15, zNPC::zNPCGroundShooter::cinfxAnimateUT15,
        zNPC::zNPCGroundShooter::cinfxRenderUT15, NULL },
      { 0, 19 } },
    { 0 }
};

} // namespace CineFXSceneUT15

void zCineFX_Register_SceneUT15()
{
    static NCINMap cutmap01[] = {
        { "ranger_opening", CineFXSceneUT15::g_cinfx_UT15_01_manager, 0 },
        { NULL, NULL, 0 }
    };

    zCineFXRegisterTable(cutmap01);
}
