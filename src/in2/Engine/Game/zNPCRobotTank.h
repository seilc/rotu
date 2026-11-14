#ifndef ZNPCROBOTTANK_H
#define ZNPCROBOTTANK_H

#include "types.h"

#include <rwcore.h>

struct zCutsceneMgr;
struct NCINEntry;
struct RpAtomic;

namespace zNPC {

class zNPCRobotTank
{
public:
    static void cinfxTankSlamUpdate(const zCutsceneMgr*, NCINEntry*, S32);
    static void cinfxTankSlamAnimate(const zCutsceneMgr*, NCINEntry*, RpAtomic*, RwMatrix*, U32, U32, U32&);
    static void cinfxTankSlamRender(const zCutsceneMgr*, NCINEntry*);
    static void cinfxTankSlamAddTweaks(const char*, NCINEntry*);
};

} // namespace zNPC

#endif
