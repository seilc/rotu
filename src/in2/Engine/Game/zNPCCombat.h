#ifndef ZNPCCOMBAT_H
#define ZNPCCOMBAT_H

#include "xEnt.h"

struct xHierarchyBoundInitData;
class zCombatAttack;

struct zNPCCombatInitData
{
    xHierarchyBoundInitData* hierarchy;
    F32 masterRadius;
    zCombatAttack* stateTable;
    U16 stateTableSize;
};

void zNPCCombatInit();
void zNPCCombatReset();
void zNPCCombatSetCombat(xEnt* ent, U32, const char* root, zNPCCombatInitData* init, U32,
                         U16 hitPoints, F32 scale);

#endif
