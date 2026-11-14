#ifndef ZNPCMELEE_H
#define ZNPCMELEE_H

#include "types.h"

#include <rwcore.h>

struct zCutsceneMgr;
struct NCINEntry;
struct RpAtomic;

namespace zNPC {

class zNPCMelee
{
public:
    static void cinfxUpdateUT13(const zCutsceneMgr*, NCINEntry*, S32);
    static void cinfxAnimateUT13(const zCutsceneMgr*, NCINEntry*, RpAtomic*, RwMatrix*, U32, U32, U32&);
    static void cinfxRenderUT13(const zCutsceneMgr*, NCINEntry*);
};

} // namespace zNPC

#endif
