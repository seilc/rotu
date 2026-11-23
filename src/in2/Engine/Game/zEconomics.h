#ifndef ZECONOMICS_H
#define ZECONOMICS_H

#include "xIni.h"

struct xEnt;

class zEconomics
{
public:
    static void Init(xIniFile* ini);
    zEconomics();
    void ParseINI(xIniFile* ini);
    void CalcLastUpgradeValue();
    S32 GetPointsToUpgrade(S32 curPoints);
    S32 GetUpgradesEarned(S32 expPoints);
    void GivePlayerXPFromNPC(S32 NPCtype, xEnt* owner, xEnt* killer);
    void GivePlayersHealthFromNPC(S32, xEnt*);
    S32 GetLevelCompletionBonus();
    void GiveLevelCompletionBonus();
    const char* GetLevelName(S32 level);
    const char* GetNPCName(S32 npc);
    S32 GetLevelNumber();

    static S32 instanceCount;
    static S32 lastUpgradeValue;
};

#endif
