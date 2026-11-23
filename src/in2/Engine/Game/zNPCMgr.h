#ifndef ZNPCMGR_H
#define ZNPCMGR_H

#include "xRMemData.h"
#include "xVec3.h"

namespace zNPC {
class zNPCCommon;
class zNPCInstance;
}

struct zNPCMgr : RyzMemData
{
    zNPC::zNPCInstance* FindNPC(U32 aid);
};

struct zSurroundedBy
{
    zNPC::zNPCCommon* npc;
    xVec3 moveToPos;
};

zNPCMgr* zNPCMgrSelf();

extern zSurroundedBy gSurroundedBy[32];
extern S32 gSurroundedByTotal;

#endif
