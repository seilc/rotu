#include "xCommon.h"
#include "zFXFrozoneSkis.h"

#include "xModel.h"
#include "xModelBucket.h"
#include "xstransvc.h"
#include "zFrozonePlayer.h"
#include "zParSysSnowsplosion.h"
#include "zShrapnel.h"

#include <rpmatfx.h>

#define FX_FROZONE_DISC_ID 0x54E8E502 // xStrHash("fx_frozone_disc")
#define ICE_GLIDE_COLLIDE_SHRAP_ID 0x3E50F6BD // xStrHash("ice_glide_collide_shrap")

namespace {

enum SkiState
{
    SKI_FADE_IN,
    SKI_ON,
    SKI_FADE_OUT,
    SKI_OFF
};

SkiState skiState = SKI_OFF;
xModelInstance* skiModel;
F32 skiTimer;
S32 sPowerupLevel;
zShrapnelAsset* collideShrap;

const zParSysSnowsplosionConfig sSnowSplosionConfig = {
    1.0f, 2.0f, 0.4f, 0.6f, 3.0f, 6.0f, 4.0f, 1.0f, 1.5f, 80, 160, 155, 0
};

} // namespace

void zFXFrozoneSkis::SetPowerupLevel(S32 level)
{
    sPowerupLevel = level;
}

void zFXFrozoneSkis::Init(zFrozonePlayer* player)
{
    RpAtomic* atomic = (RpAtomic*)xSTFindAsset(FX_FROZONE_DISC_ID, NULL);
    if (atomic) {
        skiModel = xModelInstanceAlloc(atomic, NULL, 0, 0, NULL);
        skiModel->modelID = FX_FROZONE_DISC_ID;
        if (skiModel) {
            skiModel->LightKit = player->model->LightKit;
        }
    } else {
        skiModel = NULL;
    }

    collideShrap = (zShrapnelAsset*)xSTFindAsset(ICE_GLIDE_COLLIDE_SHRAP_ID, NULL);
    Reset();
}

void zFXFrozoneSkis::Reset()
{
    skiState = SKI_OFF;
}

void zFXFrozoneSkis::Begin()
{
    if (skiState >= SKI_FADE_OUT) {
        skiState = SKI_FADE_IN;
        skiTimer = 0.0f;
    }
}

void zFXFrozoneSkis::End(bool immediate)
{
    if (skiState <= SKI_ON) {
        if (immediate) {
            skiState = SKI_OFF;
        } else {
            skiState = SKI_FADE_OUT;
            skiTimer = 0.0f;
        }
    }
}

void zFXFrozoneSkis::Update(F32 dt, zFrozonePlayer* player)
{
    if (skiState == SKI_OFF || !skiModel) {
        return;
    }

    player->GetBoneMat((xMat4x3*)skiModel->Mat, 50, (xMat4x3*)player->model->Mat);

    switch (skiState) {
    case SKI_FADE_IN:
        skiTimer += dt;
        if (skiTimer >= 0.25f) {
            skiState = SKI_ON;
        }
        break;
    case SKI_ON:
        break;
    case SKI_FADE_OUT:
        skiTimer += dt;
        if (skiTimer >= 0.25f) {
            skiState = SKI_OFF;
        }
        break;
    }
}

static RwMatrix gFXUV;

void zFXFrozoneSkis::Render()
{
    if (skiState == SKI_OFF || !skiModel) {
        return;
    }

    F32 t = (skiState == SKI_FADE_IN) ? skiTimer : 0.25f - skiTimer;
    F32 offset = (skiState == SKI_ON) ? 0.5f : 0.5f * (t / 0.25f);

    RpAtomic* atomic = skiModel->Data;
    if (atomic) {
        RpGeometry* geom = RpAtomicGetGeometry(atomic);
        if (geom) {
            RwInt32 numMaterials = RpGeometryGetNumMaterials(geom);
            for (S32 i = 0; i < numMaterials; i++) {
                RpMaterial* material = RpGeometryGetMaterial(geom, i);
                if (material) {
                    RwMatrixSetIdentity(&gFXUV);
                    gFXUV.pos.x = offset;

                    if (RpMatFXMaterialGetEffects(material) != rpMATFXEFFECTUVTRANSFORM) {
                        return;
                    }

                    RpMatFXMaterialSetEffects(material, rpMATFXEFFECTUVTRANSFORM);
                    RpMatFXMaterialSetUVTransformMatrices(material, &gFXUV, NULL);
                }
            }
        }
    }

    xModelBucket_Add(skiModel, true);
}

void zFXFrozoneSkis::CollisionEmit(xModelInstance* model)
{
    S32 count;

    switch (sPowerupLevel) {
    case 0:
        count = 3;
        break;
    case 1:
        count = 6;
        break;
    case 2:
        count = 9;
        zParSysSnowsplosionEmit(*(xVec3*)&model->Mat->pos, sSnowSplosionConfig);
        break;
    }

    while (count--) {
        collideShrap->initCB(collideShrap, model, NULL, NULL, NULL);
    }
}
