#ifndef ZNPCTYPES_H
#define ZNPCTYPES_H

#include "types.h"

enum eNPCType
{
    eNPCType_None = -1,
    eNPCType_Unknown = -1,
    eNPCType_First = 0,
    eNPCType_Common = 0,
    eNPCType_Humanoid,
    eNPCType_Rat,
    eNPCType_Chicken,
    eNPCType_Driller,
    eNPCType_Shooter,
    eNPCType_Bomber,
    eNPCType_RobotTank,
    eNPCType_Enforcer,
    eNPCType_Scientist,
    eNPCType_BossUnderminerDrill,
    eNPCType_BossUnderminerUM,
    eNPCTypeCount,
    eNPCTypeForceInt = FORCEENUMSIZEINT
};

#endif
