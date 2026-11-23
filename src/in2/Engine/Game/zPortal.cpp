#include "xCommon.h"
#include "zPortal.h"

#include "zEconomics.h"
#include "zEvent.h"
#include "zGlobals.h"
#include "zHashes.h"
#include "zPlayer.h"
#include "zScene.h"
#include "zUICustomLevelManager.h"

void zPortalInit(void* portal, void* passet)
{
    zPortalInit((zPortal*)portal, (xPortalAsset*)passet);
}

void zPortalInit(zPortal* portal, xPortalAsset* passet)
{
    xBaseInit(portal, passet);

    portal->passet = passet;
    portal->eventFunc = zPortalEventCB;

    if (portal->linkCount)
        portal->link = (xLinkAsset*)(portal->passet + 1);
}

void zPortalReset(zPortal* ent)
{
    xBaseReset(ent, ent->passet);
}

void zPortalSave(zPortal* ent, xSerial* s)
{
    xBaseSave(ent, s);
}

void zPortalLoad(zPortal* ent, xSerial* s)
{
    xBaseLoad(ent, s);
}

void zPortalEventCB(xBase*, xBase* to, U32 toEvent, const F32* toParam, xBase*, U32)
{
    switch (toEvent) {
    case eEventDebugReset:
    case eEventReset:
        zPortalReset((zPortal*)to);
        break;
    case eEventTeleportPlayer:
        if (CURPLAYERS.GetMrI()->IsDead() && CURPLAYERS.GetFrozone()->IsDead()) {
            break;
        }

        // BUG: playerNum unset when sameScene and no player widget
        S32 playerNum;

        if (*(const U32*)&toParam[4] == HASH_MRI_PLYR) {
            playerNum = 0;
        } else if (*(const U32*)&toParam[4] == HASH_FRO_PLYR) {
            playerNum = 1;
        } else if (toParam[1] == 0.0f) {
            playerNum = 0;
        }

        zSceneSwitch((zPortal*)to, toParam[0] != 0.0f, toParam[1] != 0.0f, playerNum);

        if (toParam[1] == 0.0f) {
            if (!zUIIncrediblesSceneIsCompleted(zUIIncrediblesGetCurrentSceneNumber())) {
                globals.economics->GiveLevelCompletionBonus();
            }
        }

        CURPLAYERS[playerNum]->Portaling(((zPortal*)to)->passet->sceneID);
        break;
    }
}
