#ifndef XSHADOW_H
#define XSHADOW_H

#include "xMath2.h"
#include "xMath3.h"

#include <rwcore.h>
#include <rpworld.h>

struct xEnt;
struct xModelInstance;

#define SHADOW_CACHE_ENT_MAX 16

struct xShadowPoly
{
    xVec4 vert[3];
    F32 depth;
    F32 pad[3];
};

struct xShadowCache
{
    xVec3 pos;
    xVec2 radius;
    S32 entCount;
    S32 polyCount;
    F32 polyRayDepth[5];
    U16 castOnEnt;
    U16 castOnPoly;
    F32 minDepth;
    F32 maxDepth;
    U32 pad[3];
    xEnt* ent[SHADOW_CACHE_ENT_MAX];
    xShadowPoly* poly;
};

extern RwCamera* ShadowCamera;
extern S32 SHADOW_CACHE_POLY_MAX;
extern xVec3 gShadowObjectRadius;

void xShadowInit();
void xShadowManager_Init(S32 numEnts, S32 maxPolys, S32 maxCaches);
void xShadowManager_Add(xEnt* ent);
void xShadowManager_Remove(xEnt* ent); // UNUSED
void xShadowManager_PreRender();
void xShadowManager_Render();
void xShadowSetWorld(RpWorld* world);
void xShadowSetLight(xVec3* target_pos, xVec3* in_vec, F32);
S32 xShadowCameraCreate();
void xShadowCameraUpdatePreRendered(void* model, void (*renderCB)(void*), xVec3* center, xVec3* radius, S32 preRenderedSlot, S32 shadowMode);
void xShadowCameraUpdate(void* model, void (*renderCB)(void*), xVec3* center, xVec3* radius, S32 shadowMode);
void xShadowCameraFlushPreRendered(void* model, void (*renderCB)(void*), xVec3* center, xVec3* radius, S32 preRenderedSlot, S32 shadowMode);
U32 xShadowReceiveShadowSetup(xEnt* ent);
void xShadowReceiveShadow(xEnt* ent, F32 shadowFactor, S32 shadowMode, RwMatrix* shadowMat, RwRaster* shadowRast);
void xShadow_ListAdd(xEnt* ent);
void GameCubeCameraTextureFlush(RwRaster* raster, S32 boxFilter);
void GCSaveFrameBuffer();
void GCRestoreFrameBuffer();
void xShadowVertical_FillCache(xShadowCache* cache, xVec3* pos, F32 r, F32 depth, F32 minNormY, bool castOnJSP, bool castOnEnts);
void xShadowVertical_DrawCache(xShadowCache* cache, F32 shadowFactor, F32 fadeDist, S32 shadowMode, RwMatrix* shadowMat, RwRaster* shadowRast);
void xShadowModelFileRender(RpAtomic* model, RwMatrix* animMat);

#endif
