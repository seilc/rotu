#ifndef ZSHOWERSYSTEM_H
#define ZSHOWERSYSTEM_H

#include "zLightWeight.h"
#include "xShadowSimple.h"
#include "containers.h"

struct xEnt;
struct xVec3;
struct xLightKit;
struct xModelInstance;
struct xFXRibbon;
struct xCameraFXShakeParams;
struct zShrapnelAsset;
class zFXShockwaveConfig;

class zShowerCBInterface
{
public:
    virtual void Collide(const xVec3& pos) = 0;
};

class zShowerParticle
{
public:
    struct ConfigType
    {
        zFXShockwaveConfig* shockwaveConfig;
        const xCameraFXShakeParams* cameraShakeParams;
        U32 modelID;
        U32 shrapnelID;
        F32 shrapnelSpeedY;
        F32 radius;
        F32 damageRadius;
        U16 damage;
    };

    void Render();
    void Setup(const ConfigType* cfg, xLightKit* lightKit);
    void Exit();
    void Update(pool_list<zShowerParticle>& list, xEnt* owner, zShowerCBInterface* collideCallback, F32 dt);
    void Start(const xVec3& pos, F32 fallSpeed);

protected:
    xShadowSimpleCache shadow;
    F32 speed;
    F32 time;
    const ConfigType* config;
    xModelInstance* model;
    zShrapnelAsset* shrapnel;
    xFXRibbon* ribbon;
    bool alive;
};

class zShowerSystem : public zLightweightSystemBase
{
public:
    struct ConfigType
    {
        zShowerParticle::ConfigType particleConfig;
        S32 maxParticles;
        F32 minRadius;
        F32 maxRadius;
        F32 showerTime;
        F32 minSpawnTime;
        F32 maxSpawnTime;
        F32 minSpeed;
        F32 maxSpeed;
        F32 endStormTime;
        F32 endSpawnTime;
        F32 stormDamageRadius;
    };

    void Setup(xEnt* moveOwner, const ConfigType* cfg);
    void Reset();
    void Start(const xVec3& pos, zShowerCBInterface* collideCB);
    void ResetSpawnTimer();
    void Update(F32 dt);
    void Render();
    void Exit();

protected:
    const ConfigType* config;
    pool_list<zShowerParticle> particleList;
    xVec3 center;
    F32 effectTimer;
    F32 spawnTimer;
    xEnt* owner;
    zShowerCBInterface* collideCallback;
};

#endif
