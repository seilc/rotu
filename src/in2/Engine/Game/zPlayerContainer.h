#ifndef ZPLAYERCONTAINER_H
#define ZPLAYERCONTAINER_H

#include "xDebug.h"
#include "xEnt.h"

struct zPlayer;

class zPlayerContainer
{
public:
    zPlayerContainer();

    zPlayer* operator[](S32 index)
    {
        xASSERT_L(23, index >= 0 && index < size());
        return playerArray[index];
    }

    S32 size()
    {
        return 2;
    }

    bool IsPlayer(xEnt* ptr);

private:
    zPlayer* playerArray[2];
};

#endif
