#ifndef ZMRINCREDIBLEPLAYER_H
#define ZMRINCREDIBLEPLAYER_H

#include "zCommonPlayer.h"

class zMrIncrediblePlayer : public zCommonPlayer
{
public:
    virtual void Init(xEntAsset* asset);
    virtual void SceneSetup();
    virtual void BoundUpdate(xVec3* _pos);
    virtual bool AllowInteraction(zInteraction*);
    virtual void Reset();
    virtual void Exit();
    virtual xEnt* GetCarriedEntity();
    virtual S32* GetHeadBones();
    virtual void RenderEffects();
    virtual xVec3 get_target();
    virtual bool CollideWithNPC();
    virtual F32 InitialDepenetrationVelocity();
    virtual void DefaultBeginUpdate(F32 dt);
    virtual void InitActions();

private:
    U8 unk1330[0x80];
};

#endif
