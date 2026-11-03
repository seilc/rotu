#ifndef XMODEL_H
#define XMODEL_H

#include "iModel.h"
#include "xAnim.h"
#include "xModelBucket.h"
#include "xLightKit.h"

#include <rpworld.h>

struct xSphere;
struct xSurface;
struct xLight;
template <class T> struct basic_rect;

struct xModelInstance;
struct xModelPool;

enum
{
    ModelInstance_ChildTransform = 0x40
};

static const S32 k_MAX_MODEL_LIGHTS = 4;

struct xModelInstance
{
    xModelInstance* Next;
    xModelInstance* Parent;
    xModelPool* Pool;
    xAnimPlay* Anim;
    RpAtomic* Data;
    xModelPipe Pipe;
    U8 InFrustum;
    U8 TrueClip;
    S8 sortBias;
    U8 modelpad;
    F32 RedMultiplier;
    F32 GreenMultiplier;
    F32 BlueMultiplier;
    F32 Alpha;
    F32 FadeStart;
    F32 FadeEnd;
    xSurface* Surf;
    xModelBucket** Bucket;
    xModelInstance* BucketNext;
    xLightKit* LightKit;
    xLightKit* LightKitBlend;
    F32 BlendDuration;
    F32 BlendTimeRemaining;
    void* Object;
    U16 Flags;
    U8 BoneCount;
    U8 BoneIndex;
    U8* BoneRemap;
    RwMatrix* Mat;
    xVec3 Scale;
    xBox animBound;
    xBox combinedAnimBound;
    U32 modelID;
    U32 shadowID;
    struct
    {
        xVec3* verts;
    } anim_coll;
    xLight* lights[k_MAX_MODEL_LIGHTS];
    F32 lightsDistance[k_MAX_MODEL_LIGHTS];
};

struct xModelPool
{
    xModelPool* Next;
    U32 NumMatrices;
    xModelInstance* List;
};

struct xModelPipeTable
{
    U32 ModelHashID;
    U32 SubObjectBits;
    xModelPipe Pipe;
};

struct xModelPipeLookup
{
    RpAtomic* model;
    xModelPipe Pipe;
};

extern S32 xModelPipeNumTables;
extern S32 xModelPipeCount[16];
extern xModelPipeTable* xModelPipeData[16];
extern S32 xModelLookupCount;
extern xModelPipeLookup* xModelLookupList;
extern S32 xModelInstStaticAlloc;

struct xModelTag
{
    xVec3 v;
    U32 matidx;
    F32 wt[4];
};

xModelPipe xModelGetPipe(RpAtomic* model);
void xModelInit();
void xModelInitOnce();
void xModelPoolInit(U32 count, U32 numMatrices);
xModelInstance* xModelInstanceAllocReuse(RpAtomic* data, void* object, U16 flags, U8 boneIndex, U8* boneRemap, xModelInstance* reuse_instance);
void xModelInstanceFree(xModelInstance* modelInst, bool animFree);
void xModelInstanceAttach(xModelInstance* inst, xModelInstance* parent);
void xModelUpdate(xModelInstance* modelInst, F32 timeDelta);
void xModelEvalSingle(xModelInstance* modelInst, xBox* combinedAnimBound);
void xModelEval(xModelInstance* modelInst);
void xModelRenderSingle(xModelInstance* modelInst);
void xModelRender(xModelInstance* modelInst);
void xModelRender2D(xModelInstance* modelInst, basic_rect<F32>* sr, xVec3* from, xVec3* to, F32);
void xModelRender2D(const xModelInstance& model, const basic_rect<F32>& r, const xVec3& from, const xVec3& to);
void xModel_SceneEnter(RpWorld* world);
void xModel_SceneExit(RpWorld* world);
void xModelAnimCollStart(xModelInstance& m);
void xModelAnimCollRefresh(const xModelInstance& cm);
void xModelGetBoneLocation(xVec3& loc, const xModelInstance& model, size_t index);
void xModelGetBoneMat(xMat3x3& mat, const xModelInstance& model, size_t index);
void xModelGetBoneMat(xMat4x3& mat, const xModelInstance& model, size_t index);
void xModelGetBoneMat(xMat4x3& mat, const xModelInstance& model, size_t index, const xVec3& offset);
S32 xModelSphereCull(xSphere* sphere);
S32 xModelSphereCullPlusShadow(xSphere* sphere, xVec3* shadowVec, S32* shadowOutside);
S32 xModelBoxCull(xBox* box);
S32 xModelBoxCullPlusShadow(xBox* box, xVec3* shadowVec, S32* shadowOutside);
S32 xModelCullGroup(xModelInstance* minst);
S32 xModelCullGroupPlusShadow(xModelInstance* minst, F32 shadowDepth, S32* shadowOutside);
S32 xModelCullSingle(xModelInstance* minst);
void xModelSetLightKit(xModelInstance* minst, xLightKit* lightKit, F32 transitionTime);
bool xModelIsPreinstanced(const xModelInstance* model);
bool xModelGetPipe(U32 modelID, U32 atomicNumber, xModelPipe** pipe);
RpAtomic* xModelFindAtomic(U32 id, U32* actual_id);
S32 xModelGetNumAtomics(RpAtomic& model);

inline xMat4x3* xModelGetFrame(xModelInstance* model)
{
    return (xMat4x3*)model->Mat;
}

inline void xModelSetFrame(xModelInstance* model, const xMat4x3* frame)
{
    xMat4x3Copy((xMat4x3*)model->Mat, frame);
}

bool xModelAnimCollDirty(const xModelInstance& m);

inline xModelInstance* xModelInstanceAlloc(RpAtomic* data, void* object, U16 flags, U8 boneIndex, U8* boneRemap)
{
    return xModelInstanceAllocReuse(data, object, flags, boneIndex, boneRemap, NULL);
}

inline void xModelSetMaterialAlpha(xModelInstance* modelInst, U8 alpha)
{
    iModelSetMaterialAlpha(modelInst->Data, alpha);
}

inline void xModelMaterialMul(xModelInstance* modelInst, F32 rm, F32 gm, F32 bm)
{
    iModelMaterialMul(modelInst->Data, rm, gm, bm);
}

inline void xModelResetMaterial(xModelInstance* modelInst)
{
    iModelResetMaterial(modelInst->Data);
}

extern S32 xModelBucketEnabled;
extern S32 xModelAlphaBucketEnabled;

#endif
