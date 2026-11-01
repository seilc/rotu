#ifndef ZPLAYERCONTAINER_H
#define ZPLAYERCONTAINER_H

#include "xEnt.h"

struct zPlayer;

class zPlayerContainer
{
public:
    zPlayerContainer();

    zPlayer* operator[](S32 index)
    {
        return playerArray[index];
    }

    S32 size()
    {
        return 2;
    }

    zPlayer* GetMrI()
    {
        return playerArray[0];
    }

    zPlayer* GetFrozone()
    {
        return playerArray[1];
    }

    bool IsPlayer(xEnt* ptr);

private:
    zPlayer* playerArray[2];
};

#endif
