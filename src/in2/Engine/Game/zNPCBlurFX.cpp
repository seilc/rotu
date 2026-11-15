#include "xCommon.h"
#include "zNPCBlurFX.h"

#include "xModel.h"
#include "xModelBucket.h"
#include "xRand.h"
#include "xSpacePartitionTree.h"
#include "xString.h"
#include "xstransvc.h"
#include "zNPCCommon.h"
#include "zRenderState.h"

struct zNPCBlurDriver
{
    U32 animID;
    F32 spin;
    S32 type;
    zNPCBlurDriver* next;
};

zNPCBlurDriver* gEnforcerBladeBlurDriver;
zNPCBlurDriver* gDrillerDrillBitBlurDriver;
zNPCBlurDriver* gEnforcerDrillBitBlurDriver;
zNPCBlurDriver* gBomberDrillBitBlurDriver;
xModelInstance* gBlurFXModelSpinAttack;

static zNPCBlurDriver* AddBlur(char* animName, F32 spin, S32 type)
{
    zNPCBlurDriver* driver = (zNPCBlurDriver*)xMEMALLOC(sizeof(zNPCBlurDriver), 0);

    if (animName) {
        driver->animID = xStrHash(animName);
    } else {
        driver->animID = 0;
    }

    driver->spin = spin;
    driver->type = type;
    driver->next = NULL;
    return driver;
}

static void AppendBlur(zNPCBlurDriver* driver, char* animName, F32 spin, S32 type)
{
    while (driver->next) {
        driver = driver->next;
    }
    driver->next = AddBlur(animName, spin, type);
}

void zNPCBlurFXSetup()
{
    gDrillerDrillBitBlurDriver = AddBlur("DR_L_DownCycle", 1000.0f, 3);
    AppendBlur(gDrillerDrillBitBlurDriver, "DR_L_Down", 1000.0f, 3);
    AppendBlur(gDrillerDrillBitBlurDriver, "RA_T_Over", 1000.0f, 3);
    AppendBlur(gDrillerDrillBitBlurDriver, "SA_L_Walk01", 1000.0f, 3);
    AppendBlur(gDrillerDrillBitBlurDriver, "SA_L_ShuffleB", 1000.0f, 3);
    AppendBlur(gDrillerDrillBitBlurDriver, "CS_L_Run01", 1000.0f, 3);

    gEnforcerBladeBlurDriver = AddBlur(NULL, 100.0f, 0);

    gEnforcerDrillBitBlurDriver = AddBlur("BU_L_Wait01", 1000.0f, 3);
    AppendBlur(gEnforcerDrillBitBlurDriver, "BU_T_Alert01", 1000.0f, 3);
    AppendBlur(gEnforcerDrillBitBlurDriver, "NT_S_Notice", 1000.0f, 3);
    AppendBlur(gEnforcerDrillBitBlurDriver, "RA_T_AttackCharge", 1000.0f, 3);
    AppendBlur(gEnforcerDrillBitBlurDriver, "SA_L_Walk01", 1000.0f, 3);
    AppendBlur(gEnforcerDrillBitBlurDriver, "SA_L_ShuffleL", 1000.0f, 3);
    AppendBlur(gEnforcerDrillBitBlurDriver, "SA_L_ShuffleR", 1000.0f, 3);
    AppendBlur(gEnforcerDrillBitBlurDriver, "SA_L_ShuffleB", 1000.0f, 3);
    AppendBlur(gEnforcerDrillBitBlurDriver, "SA_L_Taunt1", 1000.0f, 3);
    AppendBlur(gEnforcerDrillBitBlurDriver, "SA_L_Taunt2", 1000.0f, 3);
    AppendBlur(gEnforcerDrillBitBlurDriver, "CS_L_Run01", 1000.0f, 3);
    AppendBlur(gEnforcerDrillBitBlurDriver, "NU_L_Nuke", 1000.0f, 3);
    AppendBlur(gEnforcerDrillBitBlurDriver, "TD_T_Hit01_F", 1000.0f, 3);
    AppendBlur(gEnforcerDrillBitBlurDriver, "TD_T_Hit01_B", 1000.0f, 3);
    AppendBlur(gEnforcerDrillBitBlurDriver, "TD_T_Hit01_L", 1000.0f, 3);
    AppendBlur(gEnforcerDrillBitBlurDriver, "TD_T_Hit01_R", 1000.0f, 3);
    AppendBlur(gEnforcerDrillBitBlurDriver, "RA_T_AttackMulti", 1000.0f, 3);
    AppendBlur(gEnforcerDrillBitBlurDriver, "RA_T_AttackSwing", 1000.0f, 3);

    gBomberDrillBitBlurDriver = AddBlur("BU_L_Wait01", 1000.0f, 0);
    AppendBlur(gBomberDrillBitBlurDriver, "BU_T_Alert01", 1000.0f, 0);
    AppendBlur(gBomberDrillBitBlurDriver, "NT_S_Notice", 1000.0f, 0);
    AppendBlur(gBomberDrillBitBlurDriver, "BA_T_BeginAttack01", 1000.0f, 0);
    AppendBlur(gBomberDrillBitBlurDriver, "BA_L_AttackPause01", 1000.0f, 0);
    AppendBlur(gBomberDrillBitBlurDriver, "BA_T_Shoot01", 1000.0f, 0);
    AppendBlur(gBomberDrillBitBlurDriver, "BA_S_ShootDone01", 1000.0f, 0);
    AppendBlur(gBomberDrillBitBlurDriver, "AW_L_Away01", 1000.0f, 0);
    AppendBlur(gBomberDrillBitBlurDriver, "CS_L_Idle01", 1000.0f, 0);
    AppendBlur(gBomberDrillBitBlurDriver, "CS_T_Start01", 1000.0f, 0);
    AppendBlur(gBomberDrillBitBlurDriver, "CS_L_Run01", 1000.0f, 0);
    AppendBlur(gBomberDrillBitBlurDriver, "NU_L_Nuke", 1000.0f, 0);
}

zNPCBlurInstance* zNPCBlurFXAddInstance(S32 type, U32 modelID, S32 boneIndex, F32 x, F32 y, F32 z)
{
    zNPCBlurInstance* inst = (zNPCBlurInstance*)xMEMALLOC(sizeof(zNPCBlurInstance), 0);

    inst->type = type;
    inst->data = (RpAtomic*)xSTFindAsset(modelID, NULL);
    inst->model = NULL;
    inst->modelID = modelID;
    inst->spin = 0.0f;
    inst->boneIndex = boneIndex;
    inst->offset.assign(x, y, z);
    inst->alpha = 255.0f;
    inst->next = NULL;
    return inst;
}

void zNPCBlurFXUpdate(zNPC::zNPCCommon* npc, zNPCBlurDriver* driver, F32 dt)
{
    F32 accel = 1000.0f * dt, decel = 4000.0f * dt;

    for (zNPCBlurInstance* inst = npc->blurInstance; inst; inst = inst->next) {
        bool found = false;

        for (zNPCBlurDriver* d = driver; d; d = d->next) {
            if (d->type == inst->type &&
                (d->animID == 0 || npc->model->Anim->Single->State->ID == d->animID)) {
                found = true;

                if (d->spin > inst->spin) {
                    inst->spin += accel;
                    if (inst->spin > d->spin) {
                        inst->spin = d->spin;
                    }
                } else if (d->spin < inst->spin) {
                    inst->spin -= accel;
                    if (inst->spin < d->spin) {
                        inst->spin = d->spin;
                    }
                }

                break;
            }
        }

        if (!found) {
            if (inst->spin > 0.0f) {
                inst->spin -= decel;
                if (inst->spin <= 0.0f) {
                    inst->spin = 0.0f;
                }
            } else if (inst->spin < 0.0f) {
                inst->spin += decel;
                if (inst->spin > 0.0f) {
                    inst->spin = 0.0f;
                }
            }
        }
    }
}

void zNPCBlurFXRender(zNPC::zNPCCommon* npc)
{
    zRenderState(eRenderStateBlurCards);

    for (zNPCBlurInstance* inst = npc->blurInstance; inst; inst = inst->next) {
        if (inst->spin != 0.0f) {
            if (!inst->model) {
                inst->model = xModelInstanceAlloc(inst->data, NULL, 0, 0, NULL);
                if (inst->model) {
                    inst->model->modelID = inst->modelID;
                }
            }

            if (inst->model) {
                xMat4x3 mat, rot, result;

                xMat4x3Copy(&mat, (xMat4x3*)&npc->model->Mat[inst->boneIndex]);
                xMat4x3MoveLocalRight(&mat, inst->offset.x);
                xMat4x3MoveLocalUp(&mat, inst->offset.y);
                xMat4x3MoveLocalAt(&mat, inst->offset.z);
                xMat4x3Identity(&rot);
                xMat4x3RotY(&rot, (PI / 180.0f) * inst->spin);
                xMat4x3Mul(&result, &rot, &mat);
                xMat4x3Mul((xMat4x3*)inst->model->Mat, &result, (xMat4x3*)npc->model->Mat);
                xModelBucket_Add(inst->model, true);
            }
        } else if (inst->model) {
            xModelInstanceFree(inst->model, true);
            inst->model = NULL;
        }
    }
}

static void _WaitUpdate(zSpinBone* spinBone, F32 dt)
{
    spinBone->waitTime -= dt;

    if (spinBone->waitTime <= 0.0f) {
        spinBone->accel = xrand_RandomFloatRange(spinBone->accelMin, spinBone->accelMax);

        if (spinBone->state == ezSpinBoneState_WaitTop) {
            spinBone->state = ezSpinBoneState_SpinDown;
            spinBone->targetSpeed = xrand_RandomFloatRange(spinBone->lowSpeedMin, spinBone->lowSpeedMax);
        } else {
            spinBone->state = ezSpinBoneState_SpinUp;
            spinBone->targetSpeed = xrand_RandomFloatRange(spinBone->highSpeedMin, spinBone->highSpeedMax);
        }
    }
}

void zNPCSpinBoneUpdate(zSpinBone* spinBone, zNPC::zNPCCommon* npc, F32 dt)
{
    if (npc->freezeValue >= 1.0f) {
        return;
    }

    if (npc->freezeValue > 0.0f) {
        dt *= 1.0f - npc->freezeValue;
    }

    switch (spinBone->state) {
    case ezSpinBoneState_SpinUp:
        spinBone->speed += spinBone->accel * dt;
        if (spinBone->speed >= spinBone->targetSpeed) {
            spinBone->waitTime = xrand_RandomFloatRange(spinBone->waitMin, spinBone->waitMax);
            spinBone->state = ezSpinBoneState_WaitTop;
        }
        break;
    case ezSpinBoneState_SpinDown:
        spinBone->speed -= spinBone->accel * dt;
        if (spinBone->speed <= spinBone->targetSpeed) {
            spinBone->waitTime = xrand_RandomFloatRange(spinBone->waitMin, spinBone->waitMax);
            spinBone->state = ezSpinBoneState_WaitBottom;
        }
        break;
    case ezSpinBoneState_WaitTop:
        _WaitUpdate(spinBone, dt);
        break;
    case ezSpinBoneState_WaitBottom:
        _WaitUpdate(spinBone, dt);
        break;
    }

    spinBone->angle += spinBone->speed;
    if (spinBone->angle > 360.0f) {
        spinBone->angle -= 360.0f;
    }
}

void zNPCSpinBoneSetup(zSpinBone* spinBone, F32 accelMin, F32 accelMax, F32 lowSpeedMin, F32 lowSpeedMax,
                       F32 highSpeedMin, F32 highSpeedMax, F32 waitMin, F32 waitMax)
{
    spinBone->state = ezSpinBoneState_SpinUp;
    // BUG: reads the ranges before they're set
    spinBone->targetSpeed = xrand_RandomFloatRange(spinBone->highSpeedMin, spinBone->highSpeedMax);
    spinBone->waitTime = 0.0f;
    spinBone->speed = xrand_RandomFloatRange(spinBone->lowSpeedMin, spinBone->highSpeedMax);
    spinBone->accel = xrand_RandomFloatRange(spinBone->accelMin, spinBone->accelMax);
    spinBone->angle = 0.0f;
    spinBone->lowSpeedMin = lowSpeedMin;
    spinBone->lowSpeedMax = lowSpeedMax;
    spinBone->highSpeedMin = highSpeedMin;
    spinBone->highSpeedMax = highSpeedMax;
    spinBone->waitMin = waitMin;
    spinBone->waitMax = waitMax;
    spinBone->accelMin = accelMin;
    spinBone->accelMax = accelMax;
}

void zNPCSpinBoneBeforeAnimMatrices(zSpinBone* spinBone, S32 boneIndex, xQuat* quats, const xVec3* axis)
{
    xQuat* quat = &quats[boneIndex];
    xQuat q;

    xQuatFromAxisAngle(&q, axis, (PI / 180.0f) * spinBone->angle);
    xQuatMul(quat, quat, &q);
}
