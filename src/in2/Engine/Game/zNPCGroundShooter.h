#ifndef ZNPCGROUNDSHOOTER_H
#define ZNPCGROUNDSHOOTER_H

#include "types.h"

#include <rwcore.h>

struct zCutsceneMgr;
struct NCINEntry;
struct RpAtomic;

namespace zNPC {

class zNPCGroundShooter
{
public:
    static void cinfxUpdateUT15(const zCutsceneMgr*, NCINEntry*, S32);
    static void cinfxAnimateUT15(const zCutsceneMgr*, NCINEntry*, RpAtomic*, RwMatrix*, U32, U32, U32&);
    static void cinfxRenderUT15(const zCutsceneMgr*, NCINEntry*);
};

} // namespace zNPC

#endif
