#ifndef ZCOMBATSYSTEM_H
#define ZCOMBATSYSTEM_H

#include "types.h"

struct xEnt;

void zCombatSystemInit();
void zCombatSystemReset();
void zCombatSystemFrameStart();
void zCombatSystemUpdate(F32 dt);
void zCombatSystemRenderEffects();
void zCombatSystemUpdateEntity(xEnt* ent, F32 dt);

#endif
