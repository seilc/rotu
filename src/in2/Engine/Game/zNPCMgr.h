#ifndef ZNPCMGR_H
#define ZNPCMGR_H

#include "xRMemData.h"

namespace zNPC {
class zNPCInstance;
}

struct zNPCMgr : RyzMemData
{
    zNPC::zNPCInstance* FindNPC(U32 aid);
};

zNPCMgr* zNPCMgrSelf();

#endif
