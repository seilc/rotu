#ifndef ZNPCBLURFX_H
#define ZNPCBLURFX_H

#include "xMath3.h"

struct RpAtomic;
struct xModelInstance;
struct zNPCBlurDriver;

namespace zNPC {
class zNPCCommon;
}

struct zNPCBlurInstance
{
    S32 type;
    RpAtomic* data;
    U32 modelID;
    xModelInstance* model;
    F32 spin;
    S32 boneIndex;
    xVec3 offset;
    F32 alpha;
    zNPCBlurInstance* next;
};

enum ezSpinBoneState
{
    ezSpinBoneState_SpinUp,
    ezSpinBoneState_WaitTop,
    ezSpinBoneState_SpinDown,
    ezSpinBoneState_WaitBottom
};

struct zSpinBone
{
    ezSpinBoneState state;
    F32 targetSpeed;
    F32 waitTime;
    F32 speed;
    F32 accel;
    F32 angle;
    F32 lowSpeedMin;
    F32 lowSpeedMax;
    F32 highSpeedMin;
    F32 highSpeedMax;
    F32 waitMin;
    F32 waitMax;
    F32 accelMin;
    F32 accelMax;
};

extern zNPCBlurDriver* gEnforcerBladeBlurDriver;
extern zNPCBlurDriver* gDrillerDrillBitBlurDriver;
extern zNPCBlurDriver* gBomberDrillBitBlurDriver;
extern xModelInstance* gBlurFXModelSpinAttack;

void zNPCBlurFXSetup();
zNPCBlurInstance* zNPCBlurFXAddInstance(S32 type, U32 modelID, S32 boneIndex, F32 x, F32 y, F32 z);
void zNPCBlurFXUpdate(zNPC::zNPCCommon* npc, zNPCBlurDriver* driver, F32 dt);
void zNPCBlurFXRender(zNPC::zNPCCommon* npc);
void zNPCSpinBoneUpdate(zSpinBone* spinBone, zNPC::zNPCCommon* npc, F32 dt);
void zNPCSpinBoneSetup(zSpinBone* spinBone, F32 accelMin, F32 accelMax, F32 lowSpeedMin, F32 lowSpeedMax,
                       F32 highSpeedMin, F32 highSpeedMax, F32 waitMin, F32 waitMax);
void zNPCSpinBoneBeforeAnimMatrices(zSpinBone* spinBone, S32 boneIndex, xQuat* quats, const xVec3* axis);

#endif
