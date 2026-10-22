#include "zPlayerContainer.h"

#include "zMrIncrediblePlayer.h"
#include "zFrozonePlayer.h"

namespace {

zFrozonePlayer frozone;
zMrIncrediblePlayer mrI;

}

zPlayerContainer::zPlayerContainer()
{
    playerArray[0] = &mrI;
    playerArray[1] = &frozone;
}

bool zPlayerContainer::IsPlayer(xEnt* ptr)
{
    for (S32 i = 0; i < 2; i++) {
        if (playerArray[i] == ptr) {
            return true;
        }
    }
    return false;
}
