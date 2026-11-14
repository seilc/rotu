#ifndef ZNPCENFORCER_H
#define ZNPCENFORCER_H

#include "types.h"

#include <rwcore.h>

struct zCutsceneMgr;
struct NCINEntry;
struct RpAtomic;

namespace zNPC {

class zNPCEnforcer
{
public:
    static void cinfxSmokeUpdate(const zCutsceneMgr*, NCINEntry*, S32);
    static void cinfxSmokeAnimate(const zCutsceneMgr*, NCINEntry*, RpAtomic*, RwMatrix*, U32, U32, U32&);
    static void cinfxSmokeRender(const zCutsceneMgr*, NCINEntry*);
    static void cinfxSmokeAddTweaks(const char*, NCINEntry*);
    static void cinfxDrillTrailUpdate(const zCutsceneMgr*, NCINEntry*, S32);
    static void cinfxDrillTrailAnimate(const zCutsceneMgr*, NCINEntry*, RpAtomic*, RwMatrix*, U32, U32, U32&);
    static void cinfxDrillTrailRender(const zCutsceneMgr*, NCINEntry*);
};

} // namespace zNPC

#endif
