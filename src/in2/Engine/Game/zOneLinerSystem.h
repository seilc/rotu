#ifndef ZONELINERSYSTEM_H
#define ZONELINERSYSTEM_H

struct zPlayer;

enum zOneLinerTypes
{
    zOLT_FreezeObject = 49,
    zOLT_ShatterObject = 51,
    zOLT_Total = 55
};

void zOneLinerSystemSetup();
void zOneLinerSystemReset();
void zOneLinerSystemPlay(zPlayer*, zOneLinerTypes);

#endif
