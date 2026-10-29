#ifndef ZFROZONEPLAYER_H
#define ZFROZONEPLAYER_H

#include "zCommonPlayer.h"

class zFrozonePlayer : public zCommonPlayer
{
public:
    virtual void Init(xEntAsset* asset);
    virtual void BoundUpdate(xVec3* _pos);
    virtual void Update(F32 dt);
    virtual void EndUpdate(F32 dt);
    virtual void HandleEvent(xBase* from, U32 toEvent, const F32* toParam, xBase* toParamWidget, U32 toParamWidgetID);
    virtual void Reset();
    virtual void Exit();
    virtual S32* GetHeadBones();
    virtual void RenderEffects();
    virtual void InitActions();
};

void zPlayerFROCreatePartAtPort(S32 port);
S32 zPlayerFROGetLastUsedPad();

#endif
