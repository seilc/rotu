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
        xASSERT(index >= 0 && index < size());
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
