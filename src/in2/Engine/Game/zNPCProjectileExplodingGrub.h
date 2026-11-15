#ifndef ZNPCPROJECTILEEXPLODINGGRUB_H
#define ZNPCPROJECTILEEXPLODINGGRUB_H

#include "xDecal.h"
#include "xMath.h"
#include "xShadowSimple.h"
#include "xSnd.h"
#include "zNPCProjectile.h"

struct xModelInstance;
struct zShrapnelAsset;

#define EGRUB_MAX_PROJECTILES 10

class zNPCProjectileExplodingGrub
    : public zNPCProjectileBase<zNPCProjectileExplodingGrub, eProjectileExplodingGrub, EGRUB_MAX_PROJECTILES>
{
public:
    static void StaticSetup();

    virtual F32 GetRadius();
    virtual void Freeze();
    virtual void Reset();
    virtual void Update(F32 dt);
    virtual void Render();
    virtual void Destroy();
    virtual bool Setup(void* configData);
    void Collide(const xVec3& dpos, F32);
    virtual void Explode(bool inflictDamage, const xEnt* ent);
    virtual bool LaunchFromEnt(xEnt* ent, const xVec3* localOffset, F32 speed, xEnt* refObj);

    static xSndGroupHandle splatSoundGroup;
    static xSndGroupHandle flyingSoundGroup;
    static xSndGroupHandle launchSoundGroup;

private:
    xEnt* owner;
    xModelInstance* bugModel;
    xVec3 startPos;
    xVec3 dir;
    S32 damage;
    F32 lifetime;
    F32 speed;
    F32 emit_remaining;
    F32 spark_emit_remaining;
    F32 arc_emit_remaining;
    zShrapnelAsset* launchGoo;
    zShrapnelAsset* hitGoo;
    xDecal::emit_context decalContext;
    xResponseCurve decalResponseCurve;
    xSndHandle flyingSound;
    xShadowSimpleCache simpShadow;
};

#endif
