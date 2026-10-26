#include "zOneLinerSystem.h"

#include "xOneLiner.h"
#include "iTime.h"
#include "xRand.h"
#include "xGlobals.h"
#include "zPlayer.h"
#include "zUIIncredibles.h"

namespace
{
    xOneLiner mrIData[] =
    {
        { "MrI_melee1&2_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "MrI_melee3_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "MrI_melee4_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "MrI_take_damage_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "MrI_dodge_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "MrI_Knocked_back_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "MrI_demise_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "MrI_restart_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "MrI_long_idle_dialog", 320.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "MrI_nuke_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "MrI_switch_AOIC_dialog", 60.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "MrI_switch_player_dialog", 120.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "MrI_AI_guard_dialog", 120.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "MrI_AI_follow_dialog", 120.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "MrI_AI_natural_dialog", 120.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "MrI_spotted_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "MrI_bogart_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "MrI_sucks_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "MrI_hurt_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "MrI_surrounded_dialog", 120.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "MrI_saves_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "MrI_lift_AOIC_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "MrI_pickup_enemy_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "MrI_pickup_object_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "MrI_throw_enemy_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "MrI_throw_object_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "MrI_groundpound_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "MrI_FRO_icebridge_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "MrI_impatient_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { NULL, 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
    };

    xOneLiner frozoneData[] =
    {
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "Fro_melee1&2_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "Fro_melee3_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "Fro_melee4_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "Fro_take_damage_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "Fro_dodge_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "Fro_knockback_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "Fro_demise_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "Fro_restart_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "Fro_long_idle_dialog", 320.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "Fro_nuke_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "Fro_switch_AOIC_dialog", 60.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "Fro_AI_switch_dialog", 120.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "Fro_AI_guard_dialog", 120.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "Fro_AI_follow_dialog", 120.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "Fro_AI_natural_dialog", 120.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "Fro_spotted_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "Fro_bogart_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "Fro_sucks_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "Fro_hurt_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "Fro_surrounded_dialog", 120.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "Fro_saves_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "Fro_freeze_enemy_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "Fro_freeze_object_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "Fro_shatter_enemy_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "Fro_shatter_object_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "Fro_iceglide_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "Fro_icebridge_AOIC_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "Fro_impatient_dialog", 180.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { NULL, 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
    };

    xOneLiner bothData[] =
    {
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { "", 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
        { NULL, 0.0f, 0, 0.0f, ISND_INVALID_SND_GROUP_HANDLE },
    };
    F32 oneLinerPlayTime;
    xOneLinerList oneLiners[3];
}

void zOneLinerSystemSetup()
{
    oneLinerPlayTime = iTimeGetGame() - 20.0f;
    oneLiners[0].InitWithSounds(mrIData, zOLT_Total);
    oneLiners[1].InitWithSounds(frozoneData, zOLT_Total);
    oneLiners[2].InitWithSounds(bothData, zOLT_Total);
}

void zOneLinerSystemReset()
{
    oneLinerPlayTime = iTimeGetGame() - 20.0f;
    for (S32 r30 = 0; r30 < 2; r30++) {
        oneLiners[r30].Reset();
    }
}

void zOneLinerSystemPlay(zPlayer* r3, zOneLinerTypes r4)
{
    F32 f31 = 18.0f * zUIIncrediblesGetGameProgress() + 20.0f;
    if (iTimeGetGame() - oneLinerPlayTime < f31) {
        return;
    }

    S32 r28 = 0, r27 = -1;
    if (r4 < 21) {
        if (r3->brain_id == 13) {
            r4 = (zOneLinerTypes)(r4 + 0);
        } else {
            r4 = (zOneLinerTypes)(r4 + 21);
        }
    }

    for (S32 r26 = 0; r26 < xglobals->players.size(); r26++) {
        if (oneLiners[r26].GetSoundGroup(r4) != ISND_INVALID_SND_GROUP_HANDLE && xglobals->players[r26]->CanSayOneLiner()) {
            r27 = r26;
            r28++;
        }
    }

    bool r0 = false;
    switch (r28) {
    case 0:
        break;
    case 2:
        oneLiners[2].GetSoundGroup(r4);
        r27 = xrand_RandomChoice(3);
        r0 = oneLiners[r27].PlaySound(r4);
        break;
    case 1:
        r0 = oneLiners[r27].PlaySound(r4);
        break;
    }

    if (r0) {
        oneLinerPlayTime = iTimeGetGame();
    }
}
