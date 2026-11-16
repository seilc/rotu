#ifndef ZNPCBOSSUNDERMINERDRILL_H
#define ZNPCBOSSUNDERMINERDRILL_H

#include "zNPCCommon.h"

enum eUMBossStage
{
    eUMBossStageA,
    eUMBossStageB,
    eUMBossStageC,
    eUMBossStageD
};

class zNPCBossUnderminerDrill : public zNPC::zNPCCommon
{
public:
    U8 unk450[0x87C];
    eUMBossStage currentStage;
};

extern zNPCBossUnderminerDrill* boss;

#endif
