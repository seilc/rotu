#ifndef ZUICUSTOMHUD_H
#define ZUICUSTOMHUD_H

#include "types.h"

class zUICustomHUDManager
{
public:
    void SetHealthRatioMRI(F32, bool, bool);
    void SetHealthRatioFRO(F32, bool, bool);
};

extern zUICustomHUDManager* pUICustomHUDManager;

#endif
