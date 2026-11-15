#ifndef ZCOMMONPLAYERACTIONS_H
#define ZCOMMONPLAYERACTIONS_H

#include "zPlayerAction.h"

enum DodgeState
{
    DODGE_NONE,
    DODGE_START,
    DODGE_EXECUTE
};

enum DodgeDir
{
    DODGE_FRONT,
    DODGE_LEFT,
    DODGE_BACK,
    DODGE_RIGHT
};

class zPlayerDodge : public zPlayerAction
{
protected:
    DodgeState dodgeState;
    DodgeDir dodgeDir;
};

#endif
