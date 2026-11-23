#include "xCommon.h"
#include "zPlayerOneLinerData.h"

#include "iTime.h"
#include "xMath.h"
#include "zNPCCommon.h"
#include "zNPCMgr.h"
#include "zOneLinerSystem.h"
#include "zPlayer.h"

namespace {

xTWEAKABLE S32 SURROUND_COUNT = 6;
xTWEAKABLE S32 KILL_LIMIT = 5;
xTWEAKABLE F32 DAMAGE_TIME = 5.0f;
xTWEAKABLE S32 DAMAGE_AMOUNT = 40;

}

void zPlayerOneLinerData::AddKillCount()
{
    killCount++;
    if (killCount >= KILL_LIMIT) {
        zOneLinerSystemPlay(player, (zOneLinerTypes)0x10);
    }
}

bool zPlayerOneLinerData::Damage(S16 amount)
{
    if (amount <= 0) {
        return false;
    }

    F32 currentTime = iTimeGetGame();
    damageRecord[currentDamageIndex].time = currentTime;
    damageRecord[currentDamageIndex].damageValue = amount;

    S32 totalDamage = amount;
    for (S32 i = PrevDamageIndex(currentDamageIndex); currentTime - damageRecord[i].time <= DAMAGE_TIME && i != currentDamageIndex; i = PrevDamageIndex(i)) {
        totalDamage += damageRecord[i].damageValue;
    }

    currentDamageIndex = (currentDamageIndex + 1) % DAMAGE_RECORD_MAX;
    if (totalDamage >= DAMAGE_AMOUNT) {
        zOneLinerSystemPlay(player, (zOneLinerTypes)0x11);
        return true;
    } else {
        return false;
    }
}

void zPlayerOneLinerData::Reset()
{
    ResetKillCount();
    currentDamageIndex = 0;
    for (S32 i = 0; i < DAMAGE_RECORD_MAX; i++) {
        damageRecord[i].time = -HUGE;
    }
}

void zPlayerOneLinerData::Update(F32)
{
    S32 surroundedCount = 0;
    for (S32 i = 0; i < gSurroundedByTotal; i++) {
        if (gSurroundedBy[i].npc->perception.GetPriorityTarget() == player) {
            surroundedCount++;
        }
    }

    if (surroundedCount >= SURROUND_COUNT) {
        zOneLinerSystemPlay(player, (zOneLinerTypes)0x13);
    }
}
