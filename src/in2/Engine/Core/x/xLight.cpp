#include "xLight.h"

#include "xLightEffect.h"
#include "xstransvc.h"
#include "xModel.h"
#include "xGlobals.h"
#include "xCam.h"
#include "xGrid.h"
#include "xBound.h"
#include "xCollide.h"
#include "xQuickCull.h"
#include "xEnt.h"
#include "xMath.h"
#include "zGlobals.h"
#include "zScene.h"
#include "zEvent.h"
#include "zPlayer.h"
#include "zGrid.h"
#include "xSpacePartitionTree.h"

#include <string.h>

#define LIGHT_CARD_B_TL_ID 0x5EE75CD7 // xStrHash("light_card_b_tl")

static const S32 k_MAX_X_LIGHTS = 128;
static const S32 k_MAX_TEMP_LIGHTS = 16;
static const S32 k_MAX_LIGHT_FLASHES = 4;

namespace {
struct lightFlash
{
    xLight* light;
    F32 lifetime;
    F32 downTime;
};

xLight* sLight[k_MAX_X_LIGHTS];
S32 sLightTotal;
xLight sTempLights[k_MAX_TEMP_LIGHTS];
xLightAsset sTempLightAssets[k_MAX_TEMP_LIGHTS];
xModelInstance* sTempLightCards[k_MAX_TEMP_LIGHTS];
lightFlash sLightFlashes[k_MAX_LIGHT_FLASHES];
}

void xLightResetAll()
{
    memset(sLight, 0, sizeof(sLight));

    for (S32 i = 0; i < k_MAX_TEMP_LIGHTS; i++) {
        sTempLights[i].flags = 0;
        sLight[i] = &sTempLights[i];
    }
    sLightTotal = k_MAX_TEMP_LIGHTS;
}

void xLightSceneInit()
{
    iLightInit();
    xLightResetAll();
    xLightInitTempLights();
}

void xLightSceneExit()
{
    xLightDestroyAll();
    iLightDeinit();
}

void xLightSceneReset()
{
    for (S32 i = 0; i < k_MAX_LIGHT_FLASHES; i++) {
        sLightFlashes[i].lifetime = -1.0f;
    }
}

void xLightInit(xBase& data, xDynAsset& asset, size_t asset_size)
{
    xLightInit(&data, (xLightAsset*)&asset);
}

void xLightInit(xBase* b, xLightAsset* tasset)
{
    xLight* light = NULL;

    xBaseInit(b, tasset);

    light = (xLight*)b;
    light->eventFunc = xLightEventCB;
    light->tasset = tasset;

    if (light->linkCount)
        light->link = (xLinkAsset*)(light->tasset + 1);
    else
        light->link = NULL;

    U32 itype = 1;
    switch (tasset->lightType) {
    case eLightType_Ambient:
        itype = 1;
        break;
    case eLightType_Spotlight:
        itype = 1;
        break;
    case eLightType_Directional:
        itype = 1;
        break;
    case eLightType_Point:
        itype = 1;
        break;
    }

    if (!iLightCreate(&light->light, itype)) return;

    sLight[sLightTotal++] = (xLight*)b;

    light->lightEffect = NULL;
    light->lightCard = NULL;
    if (light->tasset->lightCardID) {
        RpAtomic* imodel = (RpAtomic*)xSTFindAsset(light->tasset->lightCardID, NULL);
        if (imodel) {
            light->lightCard = xModelInstanceAlloc(imodel, light, 0, 0, NULL);
            light->lightCard->modelID = light->tasset->lightCardID;
        }
    }
}

void xLightInitTempLights()
{
    RpAtomic* lightCardModel = (RpAtomic*)xSTFindAsset(LIGHT_CARD_B_TL_ID, NULL);
    for (S32 i = 0; i < k_MAX_TEMP_LIGHTS; i++) {
        xBaseInit(&sTempLights[i], &sTempLightAssets[i]);

        sTempLightAssets[i].lightType = eLightType_Point;
        sTempLightAssets[i].lightEffectID = 0;
        sTempLightAssets[i].lightEffectSpeed = 1.0f;
        sTempLightAssets[i].lightFlags = 0xF;
        sTempLightAssets[i].lightSphere.center = xVec3::m_Null;
        sTempLightAssets[i].lightSphere.r = 0.0f;
        xFColorCopyB(&sTempLightAssets[i].lightColor, g_WHITE);

        sTempLights[i].flags = 0;
        sTempLights[i].tasset = &sTempLightAssets[i];
        sTempLights[i].lightEffect = NULL;
        sTempLights[i].eventFunc = xLightEventCB;
        sTempLights[i].lightEffectSpeed = sTempLightAssets[i].lightEffectSpeed;
        iLightCreate(&sTempLights[i].light, 1);

        if (lightCardModel) {
            sTempLights[i].lightCardScale = 1.0f;
            sTempLightCards[i] = xModelInstanceAlloc(lightCardModel, &sTempLights[i], 0, 0, NULL);
            sTempLightCards[i]->modelID = LIGHT_CARD_B_TL_ID;
        } else {
            sTempLightCards[i] = NULL;
        }
    }

    memset(sLightFlashes, 0, sizeof(sLightFlashes));
}

xLight* xLightCreateTempInternal(bool useLightCard)
{
    for (S32 i = 0; i < k_MAX_TEMP_LIGHTS; i++) {
        if (sTempLights[i].flags & f_XLIGHT_USED) continue;

        sTempLights[i].flags = f_XLIGHT_USED;

        sTempLightAssets[i].lightFlags = 0xF;

        iLightSetPos(&sTempLights[i].light, xVec3::m_Null);
        iLightSetRadius(&sTempLights[i].light, 0.0f);

        xFColor color = { 0.0f, 0.0f, 0.0f, 0.0f };
        iLightSetColor(&sTempLights[i].light, color);

        sTempLights[i].lightEffect = NULL;
        sTempLights[i].attachedTo = NULL;
        sTempLights[i].attachedOffset.assign(0.0f);
        sTempLights[i].transTimeLeft = 0.0f;

        if (useLightCard)
            sTempLights[i].lightCard = sTempLightCards[i];
        else
            sTempLights[i].lightCard = NULL;

        return &sTempLights[i];
    }

    return NULL;
}

void xLightDestroyTempInternal(xLight* xl)
{
    if (xl->flags & f_XLIGHT_ON)
        xLightOn(xl, false, 0.0f);
    xl->flags = 0;
}

void xLightResolveLinks()
{
    for (S32 i = k_MAX_TEMP_LIGHTS; i < sLightTotal; i++) {
        xLight* xl = sLight[i];

        if (xl->tasset->attachID) {
            xl->attachedTo = zSceneFindObject(xl->tasset->attachID);
        } else {
            xl->attachedTo = NULL;
        }

        xLightSetEffect(xl, xl->tasset->lightEffectID);
    }
}

void xLightDestroyAll()
{
    S32 total = sLightTotal;
    for (S32 i = 0; i < total; i++) {
        xLightDestroy(sLight[i]);
        sLightTotal--;
    }
    sLightTotal = 0;
}

void xLightDestroy(xLight* xl)
{
    if (xl->lightCard) {
        xModelInstanceFree(xl->lightCard, true);
        xl->lightCard = NULL;
    }

    iLightDestroy(&xl->light);
}

void xLightReset(xLight* xl)
{
    xBaseReset(xl, xl->tasset);

    iLightSetPos(&xl->light, xl->tasset->lightSphere.center);
    iLightSetRadius(&xl->light, xl->tasset->lightSphere.r);

    if (xl->tasset->attachID) {
        xl->attachedOffset = xl->tasset->lightSphere.center;
    }

    iLightSetColor(&xl->light, xl->tasset->lightColor);

    xl->flags = f_XLIGHT_USED;
    if (xl->tasset->lightFlags & 0x10) {
        xl->flags |= f_XLIGHT_ON;
    }
    xl->transTimeLeft = 0.0f;

    xl->flags = f_XLIGHT_USED;
    if (xl->tasset->lightFlags & 0x10) {
        xl->flags |= f_XLIGHT_ON;
    }

    xl->lightCardScale = xl->tasset->lightCardScale;

    xLightSetEffect(xl, xl->tasset->lightEffectID);
}

void xLightEventCB(xBase*, xBase* to, U32 toEvent, const F32* toParam, xBase* toParamWidget, U32)
{
    xLight* xl = (xLight*)to;

    switch (toEvent) {
    case eEventOn:
        xLightOn(xl, true, toParam[0]);
        break;

    case eEventOff:
        xLightOn(xl, false, toParam[0]);
        break;

    case eEventToggle:
        xLightToggle(xl, toParam[0]);
        break;

    case eEventReset:
    case eEventSceneEnter:
        xLightReset(xl);
        break;

    case eEventSetColorIntensity:
        xLightSetColor(xl, toParam[0], toParam[1], toParam[2], toParam[3]);
        break;

    case eEventStartLightEffect:
    {
        bool resetEffect = toParam[0] ? true : false;

        if (toParamWidget) {
            if (toParamWidget != xl->lightEffect) {
                resetEffect = true;
            }
            xl->lightEffect = (xLightEffectBase*)toParamWidget;
        }

        if (xl->lightEffect) {
            xl->lightEffect->StartEffect(*xl, resetEffect);
        }
        break;
    }

    case eEventStopLightEffect:
    {
        bool resetColor = toParam[0] ? true : false;

        if (xl->lightEffect) {
            xl->lightEffect->StopEffect(*xl, resetColor);
        }
        break;
    }

    case eEventSetLightEffectSpeed:
        xl->lightEffectSpeed = toParam[0];
        if (xl->lightEffectSpeed <= 0.0f) {
            xl->lightEffectSpeed = 1.0f;
        }
        break;
    }
}

static void xLightUpdateLightCard(xLight* xl)
{
    if (!xl->lightCard) return;

    xVec3 pos;
    iLightGetPos(&xl->light, pos);
    xl->lightCard->Mat->pos = *(RwV3d*)&pos;

    xMat4x3 cameraMat = globals.cam->mat;
    RwMatrix* modelMat = xl->lightCard->Mat;

    F32 right_len = xl->lightCardScale * xVec3Length(modelMat->right.x, modelMat->right.y, modelMat->right.z);
    F32 up_len = xl->lightCardScale * xVec3Length(modelMat->up.x, modelMat->up.y, modelMat->up.z);
    F32 at_len = xl->lightCardScale * xVec3Length(modelMat->at.x, modelMat->at.y, modelMat->at.z);

    modelMat->at.x = cameraMat.pos.x - modelMat->pos.x; modelMat->at.y = cameraMat.pos.y - modelMat->pos.y; modelMat->at.z = cameraMat.pos.z - modelMat->pos.z;

    xVec3 _up_ = { 0.0f, 1.0f, 0.0f }, _at_ = { modelMat->at.x, modelMat->at.y, modelMat->at.z }, _right_; xVec3Cross(&_right_, &_up_, &_at_);
    xVec3Cross(&_up_, &_at_, &_right_);

    xVec3Normalize(&_at_, &_at_);
    xVec3Normalize(&_up_, &_up_);
    xVec3Normalize(&_right_, &_right_);

    modelMat->at.x = _at_.x * at_len;
    modelMat->at.y = _at_.y * at_len;
    modelMat->at.z = _at_.z * at_len;

    modelMat->up.x = _up_.x * up_len;
    modelMat->up.y = _up_.y * up_len;
    modelMat->up.z = _up_.z * up_len;

    modelMat->right.x = _right_.x * right_len;
    modelMat->right.y = _right_.y * right_len;
    modelMat->right.z = _right_.z * right_len;

    RpAtomic* lightCardAtomic = xl->lightCard->Data;
    RpGeometry* geometery = RpAtomicGetGeometry(lightCardAtomic);
    RpGeometrySetFlags(geometery, RpGeometryGetFlags(geometery) | rpGEOMETRYMODULATEMATERIALCOLOR);

    RpMaterial* material = geometery->matList.materials[0];

    xFColor lightColor;
    iLightGetColor(&xl->light, lightColor);
    F32 maxColor = MAX(lightColor.r, MAX(lightColor.g, lightColor.b));
    F32 colorMultiplier = maxColor ? 255.0f / maxColor : 0.0f;

    RwRGBA color;
    color.red = lightColor.r * colorMultiplier;
    color.blue = lightColor.b * colorMultiplier;
    color.green = lightColor.g * colorMultiplier;
    color.alpha = 255.0f * maxColor;
    RpMaterialSetColor(material, &color);
}

void xLightUpdateAll(F32 dt)
{
    S32 i;

    for (i = 0; i < k_MAX_LIGHT_FLASHES; i++) {
        if (sLightFlashes[i].light) {
            lightFlash& flash = sLightFlashes[i];

            flash.lifetime -= dt;
            if (flash.lifetime < 0.0f) {
                xLightDestroyTemp(flash.light);
                flash.light = NULL;
            }
            else if (flash.lifetime < flash.downTime) {
                xLightOn(flash.light, false, flash.lifetime);
                flash.downTime = -flash.downTime;
            }
        }
    }

    for (i = 0; i < sLightTotal; i++) {
        xLight* xl = sLight[i];

        if (xl->flags & f_XLIGHT_ON) {
            if (xl->lightEffect && xl->lightEffectRunning) {
                if (xl->lightEffectSpeed != 0.0f) {
                    xl->lightEffect->UpdateEffect(*xl, dt / xl->lightEffectSpeed);
                }
            }

            if (xl->transTimeLeft) {
                xl->transTimeLeft -= dt;
                if (xl->transTimeLeft <= 0.0f) {
                    xl->transTimeLeft = 0.0f;

                    if (!xl->endColor.a) {
                        iLightSetColor(&xl->light, xl->startColor);
                        xLightOn(xl, false, 0.0f);
                    } else {
                        iLightSetColor(&xl->light, xl->endColor);
                    }
                } else {
                    F32 percentage = xl->transTimeLeft / xl->totalTransTime;
                    xFColor tempColor = {
                        percentage * (xl->startColor.r - xl->endColor.r) + xl->endColor.r,
                        percentage * (xl->startColor.g - xl->endColor.g) + xl->endColor.g,
                        percentage * (xl->startColor.b - xl->endColor.b) + xl->endColor.b,
                        percentage * (xl->startColor.a - xl->endColor.a) + xl->endColor.a
                    };
                    iLightSetColor(&xl->light, tempColor);
                }
            }

            if (xl->attachedTo) {
                xVec3 pos;
                xMat4x3 matrix = *xEntGetFrame((xEnt*)xl->attachedTo);
                xMat4x3ToWorld(&pos, &matrix, &xl->attachedOffset);
                iLightSetPos(&xl->light, pos);
                xl->light.sph.center = pos;
            }
        }
    }
}

void xLightRenderAll()
{
    for (S32 i = 0; i < sLightTotal; i++) {
        xLight* xlight = sLight[i];

        if (xlight->flags & f_XLIGHT_ON) {
            if (xlight->lightCard) {
                if (xlight->lightCard->Mat) {
                    RwMatrix modelMat = *xlight->lightCard->Mat;

                    xLightUpdateLightCard(xlight);
                    xModelRender(xlight->lightCard);

                    *xlight->lightCard->Mat = modelMat;
                }
            }
        }
    }
}

void xLightAddLocalEnv(xEnv* env)
{
    for (S32 i = 0; i < sLightTotal; i++) {
        xLight* xlight = sLight[i];

        if (!(xlight->flags & f_XLIGHT_USED)) continue;

        if (xlight->flags & f_XLIGHT_ON) {
            if (xlight->tasset->lightFlags & 0x8) {
                iLight* light = &xlight->light;
                iLightEnv(light, 3);
                iLightJSPPreRender(env, light);
            }
        }
    }
}

void xLightRemoveLocalEnv(xEnv* env)
{
    iLightJSPPostRender(env);
}

namespace {
class xLightCollisionCallback
{
public:
    xLightCollisionCallback(const xSphere& sphere, xLight* light) : sphere(&sphere), light(light) {}

    bool operator()(xEnt& ent)
    {
        if (!ent.model || !xEntIsVisible(&ent)) {
            return true;
        }

        xCollis coll;
        coll.flags = 0x1F00;
        xSphereHitsBound(sphere, &ent.bound, &coll);
        if (!(coll.flags & 0x1)) {
            return true;
        }

        xModelInstance* model = ent.model;
        while (model) {
            if (model->lights[k_MAX_MODEL_LIGHTS - 1]) {
                S32 furthestLight = 0;
                for (S32 i = 0; i < k_MAX_MODEL_LIGHTS; i++) {
                    if (model->lights[i] == light) {
                        return true;
                    }

                    if (model->lightsDistance[i] > model->lightsDistance[furthestLight]) {
                        furthestLight = i;
                    }
                }

                if (model->lightsDistance[furthestLight] > coll.dist) {
                    model->lightsDistance[furthestLight] = coll.dist;
                    model->lights[furthestLight] = light;
                }
            } else {
                S32 freeLight = 0;
                while (model->lights[freeLight]) {
                    if (model->lights[freeLight] == light) {
                        return true;
                    }
                    freeLight++;
                }
                model->lightsDistance[freeLight] = coll.dist;
                model->lights[freeLight] = light;
            }
            model = model->Next;
        }
        return true;
    }

private:
    const xSphere* sphere;
    xLight* light;
};
}

void xLightPrepLocal()
{
    for (S32 i = 0; i < sLightTotal; i++) {
        xLight* xlight = sLight[i];

        if (!(xlight->flags & f_XLIGHT_USED)) continue;

        if (xlight->flags & f_XLIGHT_ON) {
            const xSphere* lightSphere = iLightGetBoundingSphere(&xlight->light);

            xBound tempBound;
            tempBound.type = k_XBOUNDTYPE_SPHERE;
            xQuickCullForSphere(&tempBound.qcd, lightSphere);
            tempBound.sph = *lightSphere;

            xLightCollisionCallback static_callback(*lightSphere, xlight);

            if (xlight->tasset->lightFlags & 0x4) {
                xGridCheckBound(colls_grid, tempBound, tempBound.qcd, static_callback);
                xGridCheckBound(colls_oso_grid, tempBound, tempBound.qcd, static_callback);
                xGridCheckBound(grabbable_grid, tempBound, tempBound.qcd, static_callback);
            }

            if (xlight->tasset->lightFlags & 0x1) {
                xGridCheckBound(npcs_grid, tempBound, tempBound.qcd, static_callback);
            }

            if (xlight->tasset->lightFlags & 0x2) {
                for (S32 i = 0; i < xglobals->players.size(); i++) {
                    xSphere* extra_spheres[10];
                    S32 numSpheres = xglobals->players[i]->GetCollisionSpheres((const xSphere**)extra_spheres);

                    for (S32 j = 0; j < numSpheres; j++) {
                        xCollis coll;
                        coll.flags = 0;
                        xSphereHitsSphere(lightSphere, extra_spheres[j], &coll);
                        if (coll.flags & 0x1) {
                            static_callback(*xglobals->players[i]);
                            break;
                        }
                    }
                }
            }
        }
    }
}

void xLightAddLocal(xModelInstance* minst)
{
    xLight** xlights = minst->lights;

    if (!xlights[0]) return;

    iLight* ilights[k_MAX_MODEL_LIGHTS];
    S32 i;

    for (i = 0; xlights[i] && i < k_MAX_MODEL_LIGHTS; i++)
        ilights[i] = &xlights[i]->light;

    iLightAddForModel(minst, ilights, i);
}

void xLightRemoveLocal(xModelInstance* minst)
{
    xLight** xlights = minst->lights;

    if (!xlights[0]) return;

    iLight* ilights[k_MAX_MODEL_LIGHTS];
    S32 i;

    for (i = 0; xlights[i] && i < k_MAX_MODEL_LIGHTS; i++)
        ilights[i] = &xlights[i]->light;

    iLightRemoveFromModel(minst, ilights, i);
    memset(xlights, 0, sizeof(minst->lights));
}

void xLightSetEffect(xLight* xl, U32 effectID)
{
    xl->lightEffect = (xLightEffectBase*)zSceneFindObject(effectID);

    xLightResetEffect(xl);
}

void xLightResetEffect(xLight* xl)
{
    if (xl->lightEffect) {
        xl->lightEffectSpeed = xl->tasset->lightEffectSpeed;
        if (xl->lightEffectSpeed <= 0.0f)
            xl->lightEffectSpeed = 1.0f;
        xl->lightCardScale = xl->tasset->lightCardScale;

        xl->lightEffect->StartEffect(*xl, true);
    }
}

void xLightOn(xLight* xl, bool on, F32 transitionTime)
{
    xl->transTimeLeft = xl->totalTransTime = transitionTime;

    if (on) {
        xl->flags |= f_XLIGHT_ON;

        if (transitionTime) {
            iLightGetColor(&xl->light, xl->endColor);
            xFColorInit(&xl->startColor, 0.0f, 0.0f, 0.0f, 1.0f);

            iLightSetColor(&xl->light, xl->startColor);
        }
    } else {
        if (transitionTime) {
            iLightGetColor(&xl->light, xl->startColor);
            xFColorInit(&xl->endColor, 0.0f, 0.0f, 0.0f, 0.0f);
        } else {
            xl->flags &= ~f_XLIGHT_ON;
        }
    }
}

void xLightToggle(xLight* xl, F32 transitionTime)
{
    if (xl->flags & f_XLIGHT_ON)
        xLightOn(xl, false, transitionTime);
    else
        xLightOn(xl, true, transitionTime);
}

void xLightSetColor(xLight* xl, F32 r, F32 g, F32 b, F32 transitionTime)
{
    xFColor col = { r, g, b, 1.0f };
    xLightSetColor(xl, col, transitionTime);
}

void xLightSetColor(xLight* xl, const xFColor& col, F32 transitionTime)
{
    if (transitionTime) {
        iLightGetColor(&xl->light, xl->startColor);
        xl->endColor = col;
        xl->transTimeLeft = xl->totalTransTime = transitionTime;
    } else {
        xl->transTimeLeft = 0.0f;
        iLightSetColor(&xl->light, col);
    }
}

void xLightGetColor(xLight* xl, xFColor& col)
{
    iLightGetColor(&xl->light, col);
}

void xLightSetPos(xLight* xl, const xVec3& pos)
{
    iLightSetPos(&xl->light, pos);
}

void xLightSetRadius(xLight* xl, F32 radius)
{
    iLightSetRadius(&xl->light, radius);
}

void xLightSetLightCardScale(xLight* xl, F32 scale)
{
    xl->lightCardScale = scale;
}

void xLightFlash(const xVec3& pos, const xFColor& color, F32 radius, F32 upTime, F32 maxTime, F32 downTime)
{
    lightFlash* flash = NULL;
    for (S32 i = 0; i < k_MAX_LIGHT_FLASHES; i++) {
        if (!sLightFlashes[i].light) {
            flash = &sLightFlashes[i];
            break;
        }
    }

    if (!flash) return;

    flash->light = xLightCreateTemp(false);
    if (!flash->light) return;

    flash->downTime = downTime;
    flash->lifetime = downTime + (upTime + maxTime);

    xLightSetPos(flash->light, pos);
    xLightSetColor(flash->light, color, 0.0f);
    xLightSetRadius(flash->light, radius);
    xLightOn(flash->light, true, upTime);
}

