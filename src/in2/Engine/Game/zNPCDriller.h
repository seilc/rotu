#ifndef ZNPCDRILLER_H
#define ZNPCDRILLER_H

#include "types.h"

#include <rwcore.h>

struct zCutsceneMgr;
struct NCINEntry;
struct RpAtomic;

namespace zNPC {

class zNPCDriller
{
public:
    static void cinfxDrillTunnelUpUpdate(const zCutsceneMgr*, NCINEntry*, S32);
    static void cinfxDrillTunnelUpAnimate(const zCutsceneMgr*, NCINEntry*, RpAtomic*, RwMatrix*, U32, U32, U32&);
    static void cinfxDrillTunnelUpRender(const zCutsceneMgr*, NCINEntry*);
    static void cinfxDrillTunnelUpAddTweaks(const char*, NCINEntry*);
    static void cinfxDrillTunnelDownUpdate(const zCutsceneMgr*, NCINEntry*, S32);
    static void cinfxDrillTunnelDownAnimate(const zCutsceneMgr*, NCINEntry*, RpAtomic*, RwMatrix*, U32, U32, U32&);
    static void cinfxDrillTunnelDownRender(const zCutsceneMgr*, NCINEntry*);
    static void cinfxDrillTunnelDownAddTweaks(const char*, NCINEntry*);
    static void cinfxDrillTunnelPlumeUpdate(const zCutsceneMgr*, NCINEntry*, S32);
    static void cinfxDrillTunnelPlumeAnimate(const zCutsceneMgr*, NCINEntry*, RpAtomic*, RwMatrix*, U32, U32, U32&);
    static void cinfxDrillTunnelPlumeRender(const zCutsceneMgr*, NCINEntry*);
    static void cinfxDrillTunnelPlumeAddTweaks(const char*, NCINEntry*);
    static void cinfxDrillerLandUpdate(const zCutsceneMgr*, NCINEntry*, S32);
    static void cinfxDrillerLandAnimate(const zCutsceneMgr*, NCINEntry*, RpAtomic*, RwMatrix*, U32, U32, U32&);
    static void cinfxDrillerLandRender(const zCutsceneMgr*, NCINEntry*);
    static void cinfxDrillerLandAddTweaks(const char*, NCINEntry*);
};

} // namespace zNPC

#endif
