#include "xCommon.h"
#include "zCineFXTableSceneUT13.h"

#include "zCineFX.h"
#include "zCineFXShared.h"
#include "zNPCMelee.h"

namespace CineFXSceneUT13 {

static NCINEntry g_cinfx_UT13_01_manager[] = {
    { { 0x1000, CINFX_FRAMES(75), CINFX_FRAMES(90), "UT13 shockwave" },
      { zNPC::zNPCMelee::cinfxUpdateUT13, zNPC::zNPCMelee::cinfxAnimateUT13,
        zNPC::zNPCMelee::cinfxRenderUT13, NULL },
      { 0, 17 } },
    { 0 }
};

} // namespace CineFXSceneUT13

void zCineFX_Register_SceneUT13()
{
    static NCINMap cutmap01[] = {
        { "mole_bot_opening", CineFXSceneUT13::g_cinfx_UT13_01_manager, 0 },
        { NULL, NULL, 0 }
    };

    zCineFXRegisterTable(cutmap01);
}
