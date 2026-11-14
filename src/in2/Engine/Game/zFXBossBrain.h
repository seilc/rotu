#ifndef ZFXBOSSBRAIN_H
#define ZFXBOSSBRAIN_H

#include "types.h"

#include <rwcore.h>

struct zCutsceneMgr;
struct NCINEntry;
struct RpAtomic;

struct cinfx_bossbrain_context
{
    U32 raster_id;
    S32 num_cards;
    F32 card_time;
    F32 scale;
    U32 dome_id;
    F32 dome_scale;
    F32 dome_height;
};

class zFXBossBrain
{
public:
    static void cinfxUpdate(const zCutsceneMgr*, NCINEntry*, S32);
    static void cinfxAnimate(const zCutsceneMgr*, NCINEntry*, RpAtomic*, RwMatrix*, U32, U32, U32&);
    static void cinfxRender(const zCutsceneMgr*, NCINEntry*);
    static void cinfxAddTweaks(const char*, NCINEntry*);
};

#endif
