#ifndef ZFXBOSSBRAIN_H
#define ZFXBOSSBRAIN_H

#include "xBase.h"
#include "xMath3.h"
#include "zFXBossBrainAsset.h"

#include <rwcore.h>

struct xEnt;
struct xModelInstance;
struct zCutsceneMgr;
struct NCINEntry;
struct RpAtomic;

#define MAX_BRAIN_CARDS 20

// zFXBossBrain::card::flags
#define k_ZFXBOSSBRAINCARD_U_HALF 0x1
#define k_ZFXBOSSBRAINCARD_V_HALF 0x2
#define k_ZFXBOSSBRAINCARD_FLIP_U 0x4
#define k_ZFXBOSSBRAINCARD_FLIP_V 0x8
#define k_ZFXBOSSBRAINCARD_QUARTER 0x3
#define k_ZFXBOSSBRAINCARD_ALL 0xF

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

class zFXBossBrain : public xBase
{
public:
    struct card
    {
        xMat3x3 mat;
        F32 time;
        U32 flags;
    };

    card mCards[MAX_BRAIN_CARDS];
    bool mOn;
    zFXBossBrainAsset* mAsset;
    xEnt* mAttachEnt;
    U32 mAttachBone;
    xVec3 mPos;
    S32 mNumCards;
    F32 mCardTime;
    F32 mScale;
    RwRaster* mRaster;
    xModelInstance* mDome;

    static void Init(xBase& data, xDynAsset& asset, size_t);
    void Init(zFXBossBrainAsset* asset);
    void Setup();
    void Reset();
    static void randomizeCard(card& c, F32 time, bool full);
    void AttachBaseTo(xBase* base, S32 bone);
    void AttachTo(xEnt* ent, S32 bone);
    static void UpdateAll(F32 dt);
    void Update(F32 dt);
    static void updateCards(card* cards, S32 count, F32 time, F32 dt);
    static void EarlyRenderAll();
    static void RenderAll();
    void Render();
    static void render(const card* cards, S32 count, const xVec3& pos, F32 scale, RwRaster* raster);
    static void Event(xBase* from, xBase* to, U32 toEvent, const F32* toParam, xBase* toParamWidget,
                      U32 toParamWidgetID);
    void Event(xBase* from, U32 toEvent, const F32* toParam, xBase* toParamWidget, U32 toParamWidgetID);

    static void cinfxUpdate(const zCutsceneMgr*, NCINEntry*, S32);
    static void cinfxAnimate(const zCutsceneMgr*, NCINEntry*, RpAtomic*, RwMatrix*, U32, U32, U32&);
    static void cinfxRender(const zCutsceneMgr*, NCINEntry*);
    static void cinfxAddTweaks(const char*, NCINEntry*);
};

#endif
