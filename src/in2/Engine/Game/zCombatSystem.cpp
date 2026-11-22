#include "xCommon.h"
#include "zCombatSystem.h"

#include "zCombat.h"
#include "zGlobals.h"

enum
{
    MAX_COMBAT = 96
};

namespace {
xEnt* combatList[MAX_COMBAT];
U32 currentCombat;
} // namespace

void zCombatSystemInit()
{
    zCombatSystemReset();
}

void zCombatSystemReset()
{
    currentCombat = 0;
    globals.boundUpdateTime = 0;
}

void zCombatSystemFrameStart()
{
    currentCombat = 0;
}

void zCombatSystemUpdate(F32 dt)
{
    if (currentCombat == 0) {
        return;
    }

    globals.boundUpdateTime++;

    for (U32 i = 0; i < currentCombat; i++) {
        xEnt* ent = combatList[i];
        zCombat* combat = zCombatGetFrom(ent);

        combat->PostUpdate(ent, dt);
    }
}

void zCombatSystemRenderEffects()
{
    if (currentCombat == 0) {
        return;
    }

    for (U32 i = 0; i < currentCombat; i++) {
        xEnt* ent = combatList[i];
        zCombat* combat = zCombatGetFrom(ent);

        combat->RenderAfterEffect(ent);
    }
}

void zCombatSystemUpdateEntity(xEnt* ent, F32 dt)
{
    if (!ent->model->Anim || !ent->model->Anim->Single->State) {
        return;
    }

    zCombat* combat = zCombatGetFrom(ent);

    combat->PreUpdate(ent, dt);

    if (currentCombat < MAX_COMBAT)
        combatList[currentCombat++] = ent;
}
