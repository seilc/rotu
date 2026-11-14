#ifndef ZNPCBOMBER_H
#define ZNPCBOMBER_H

#include "types.h"

#include <rwcore.h>

struct zCutsceneMgr;
struct NCINEntry;
struct RpAtomic;

namespace zNPC {

class zNPCBomber
{
public:
    static void cinfxDrillTrailUpdate(const zCutsceneMgr*, NCINEntry*, S32);
    static void cinfxDrillTrailAnimate(const zCutsceneMgr*, NCINEntry*, RpAtomic*, RwMatrix*, U32, U32, U32&);
    static void cinfxDrillTrailRender(const zCutsceneMgr*, NCINEntry*);
    static void cinfxDrillTrailAddTweaks(const char*, NCINEntry*);
    static void cinfxShieldUpdate(const zCutsceneMgr*, NCINEntry*, S32);
    static void cinfxShieldAnimate(const zCutsceneMgr*, NCINEntry*, RpAtomic*, RwMatrix*, U32, U32, U32&);
    static void cinfxShieldRender(const zCutsceneMgr*, NCINEntry*);
    static void cinfxShieldAddTweaks(const char*, NCINEntry*);
    static void cinfxLaserBlastsUpdate(const zCutsceneMgr*, NCINEntry*, S32);
    static void cinfxLaserBlastsAnimate(const zCutsceneMgr*, NCINEntry*, RpAtomic*, RwMatrix*, U32, U32, U32&);
    static void cinfxLaserBlastsRender(const zCutsceneMgr*, NCINEntry*);
    static void cinfxLaserBlastsAddTweaks(const char*, NCINEntry*);
    static void cinfxSmokeUpdate(const zCutsceneMgr*, NCINEntry*, S32);
    static void cinfxSmokeAnimate(const zCutsceneMgr*, NCINEntry*, RpAtomic*, RwMatrix*, U32, U32, U32&);
    static void cinfxSmokeRender(const zCutsceneMgr*, NCINEntry*);
    static void cinfxSmokeAddTweaks(const char*, NCINEntry*);
    static void cinfxLightUpdate(const zCutsceneMgr*, NCINEntry*, S32);
    static void cinfxLightAnimate(const zCutsceneMgr*, NCINEntry*, RpAtomic*, RwMatrix*, U32, U32, U32&);
    static void cinfxLightRender(const zCutsceneMgr*, NCINEntry*);
    static void cinfxLightAddTweaks(const char*, NCINEntry*);
    static void cinfxDrillClodsUpdate(const zCutsceneMgr*, NCINEntry*, S32);
    static void cinfxDrillClodsAnimate(const zCutsceneMgr*, NCINEntry*, RpAtomic*, RwMatrix*, U32, U32, U32&);
    static void cinfxDrillClodsRender(const zCutsceneMgr*, NCINEntry*);
    static void cinfxDrillClodsAddTweaks(const char*, NCINEntry*);
    static void cinfxCraterDecalUpdate(const zCutsceneMgr*, NCINEntry*, S32);
    static void cinfxCraterDecalAnimate(const zCutsceneMgr*, NCINEntry*, RpAtomic*, RwMatrix*, U32, U32, U32&);
    static void cinfxCraterDecalRender(const zCutsceneMgr*, NCINEntry*);
    static void cinfxCraterDecalAddTweaks(const char*, NCINEntry*);
};

} // namespace zNPC

#endif
