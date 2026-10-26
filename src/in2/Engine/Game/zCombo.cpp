#include "zCombo.h"

#include "xHudText.h"

#include "decomp.h"

static xhud::text_widget::widget_chunk* comboHUD = NULL;
static bool sComboIsPaused = false;

void zCombo_Add(F32 points, zComboType type)
{
    S32 rewardLevel;
    DECOMP_FORCEREGISTER(rewardLevel);
    (void)points;
    (void)type;
}

DECOMP_FORCEACTIVE(
    "NumCombos",
    "ComboTimer",
    "ComboDisplayTime",
    "ComboFadeDir",
    "leftright",
    "updown",
    DEBUG ? "zCombo.cpp" : 0,
    "Combo%02d",
    "HUD_TEXT_COMBOMESSAGE"
)

void zCombo_Paused()
{
    if (sComboIsPaused) {
        return;
    }

    sComboIsPaused = true;

    if (comboHUD) {
        comboHUD->w.set_text("");
        comboHUD->w.clear_motives();
    }
}
