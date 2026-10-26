#ifndef ZONELINERSYSTEM_H
#define ZONELINERSYSTEM_H

#include "types.h"

struct zPlayer;

enum zOneLinerTypes
{
    zOLT_Total = 55
};

void zOneLinerSystemSetup();
void zOneLinerSystemReset();
void zOneLinerSystemPlay(zPlayer*, zOneLinerTypes);

#endif
