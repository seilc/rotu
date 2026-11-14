#include "xCommon.h"
#include "zNPCCombat.h"

#include "zAnimCache.h"
#include "zCombat.h"

void zNPCCombatInit()
{
    zNPCCombatReset();
}

void zNPCCombatReset()
{
}

void zNPCCombatSetCombat(xEnt* ent, U32, const char* root, zNPCCombatInitData* init, U32,
                         U16 hitPoints, F32 scale)
{
    ent->user_data = xMEMALLOC(sizeof(zCombat), 0);
    zCombat* combat = zCombatGetFrom(ent);

    if (init->stateTable != NULL) {
        zAnimCacheCreateData(init->stateTable, init->stateTableSize, ent->model->Anim);
    }

    combat->Init(init->hierarchy, init->masterRadius, init->stateTable, init->stateTableSize,
                 hitPoints, root, NULL, ent->model->BoneCount, scale);
    combat->envCollisionCB = NULL;
}
