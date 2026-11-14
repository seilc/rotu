#ifndef ZANIMCACHE_H
#define ZANIMCACHE_H

#include "xAnim.h"

class zCombatAttack;

void zAnimCacheCreateData(zCombatAttack* attackTableStates, U32 attackTableCount,
                          xAnimPlay* animPlay);

#endif
