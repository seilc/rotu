#ifndef ZUICUSTOMARTWORKMANAGER_H
#define ZUICUSTOMARTWORKMANAGER_H

#include "zUICustom.h"

class zUICustomArtworkManager : public zUICustom
{
public:
    void LockAll();
};

extern zUICustomArtworkManager* pUICustomArtworkManager;

#endif
