#include "xCommon.h"
#include "zFXFreezeNPC.h"

#include "iModel.h"
#include "xEnt.h"
#include "xGlobals.h"
#include "xMath3.h"
#include "xModel.h"
#include "xRand.h"
#include "xstransvc.h"
#include "xString.h"
#include "xutil.h"
#include "xVec3.h"
#include "zFXGlint.h"
#include "zGlobals.h"
#include "zHashes.h"
#include "zNPCCommon.h"
#include "zPlayer.h"
#include "zShrapnel.h"

#include <rpmatfx.h>
#include <string.h>

#define MAX_FREEZING_FX 8

namespace {

U32 type_counts[MAX_ICE_TYPES] = { 8, 2, 4 };
const xVec3 ice_scale = { 1.0f, 1.0f, 1.0f };
const xVec3 ice_scaling_axes = { 1.0f, 0.0f, 1.0f };

} // namespace

static zFXFreezeNPC sFreezeNPC;
static zFX::glint::config glint_config;

zFXFreezeNPC::block* zFXFreezeNPC::find_block(xEnt* ent, U32 type)
{
    block* b = blocks[type];
    for (U32 i = 0; i < type_counts[type]; i++, b++) {
        if (b->ent == ent) {
            return b;
        }
    }
    return NULL;
}

bool zFXFreezeNPC::HasBlockForEnt(xEnt* ent, U32 type)
{
    return sFreezeNPC.find_block(ent, type) != NULL;
}

zFXFreezeNPC::freezing_fx* zFXFreezeNPC::find_freezing_fx(xEnt* ent)
{
    for (U32 i = 0; i < MAX_FREEZING_FX; i++) {
        if (freezing[i].ent == ent) {
            return &freezing[i];
        }
    }
    return NULL;
}

void zFXFreezeNPC::Init()
{
    sFreezeNPC._Init();
}

void zFXFreezeNPC::_Init()
{
    frozoneIndex = -1;
    for (S32 i = 0; i < CURPLAYERS.size(); i++) {
        if (CURPLAYERS[i]->brain_id == 14) {
            frozoneIndex = i;
        }
    }

    if (frozoneIndex == -1) {
        return;
    }

    thawShrapnel[0] = (zShrapnelAsset*)xSTFindAsset(xStrHash("ice_thaw"), NULL);
    thawShrapnel[1] = (zShrapnelAsset*)xSTFindAsset(xStrHash("ice_thaw_tank"), NULL);
    thawShrapnel[2] = (zShrapnelAsset*)xSTFindAsset(xStrHash("ice_thaw_enforcer"), NULL);
    shatterShrapnel[0] = (zShrapnelAsset*)xSTFindAsset(xStrHash("ice_shatter"), NULL);
    shatterShrapnel[1] = (zShrapnelAsset*)xSTFindAsset(xStrHash("ice_shatter_tank"), NULL);
    shatterShrapnel[2] = (zShrapnelAsset*)xSTFindAsset(xStrHash("ice_shatter_enforcer"), NULL);

    for (U32 i = 0; i < MAX_ICE_TYPES; i++) {
        blocks[i] = (block*)xMEMALLOC(type_counts[i] * sizeof(block), 0);
    }

    U32 modelIDs[MAX_ICE_TYPES] = { HASH_iced_foe_block, HASH_fx_tank_ice_block, HASH_iced_enf_block_dff };
    RpAtomic* models[MAX_ICE_TYPES] = {
        (RpAtomic*)xSTFindAsset(modelIDs[0], NULL),
        (RpAtomic*)xSTFindAsset(modelIDs[1], NULL),
        (RpAtomic*)xSTFindAsset(modelIDs[2], NULL)
    };
    U32 decalIDs[MAX_ICE_TYPES] = { HASH_frost_decal, 0, HASH_frost_decal };
    F32 decalSizes[MAX_ICE_TYPES] = { 2.0f, 0.0f, 4.0f };

    for (U32 i = 0; i < MAX_ICE_TYPES; i++) {
        if (models[i]) {
            block* b = blocks[i];
            for (U32 j = 0; j < type_counts[i]; j++, b++) {
                // BUG: clears only the first 4 bytes
                memset(b, 0, sizeof(b));
                b->ent = NULL;
                b->model = xModelInstanceAlloc(models[i], this, 0, 0, NULL);
                b->model->modelID = modelIDs[i];
            }
        }
    }

    freezing = (freezing_fx*)xMEMALLOC(MAX_FREEZING_FX * sizeof(freezing_fx), 0);
    // BUG: the curve as its own nodes
    iceCurve.reset(6, &iceCurve, 4);

    InitDebugTweak();

    zFX::glint::setup();
    glint_config = zFX::glint::default_config;
    glint_config.emit_rate = 2.5f;
    glint_config.fade_in = 0.4f;
    glint_config.fade_out = 0.6f;
    glint_config.min_life = 0.1f;
    glint_config.max_life = 0.25f;
    glint_config.min_size = 0.15f;
    glint_config.max_size = 0.4f;
    glint_config.size_delta = 1.5f;

    for (U32 i = 0; i < MAX_ICE_TYPES; i++) {
        if (decalIDs[i]) {
            static const xDecal::curve_node decalCurve[] = {
                { 0.0f, { 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f } },
                { 0.04f, { 1.0f, 1.0f, 1.0f, 0.8f, 1.0f, 1.0f } },
                { 0.64f, { 1.0f, 1.0f, 1.0f, 0.8f, 1.0f, 1.0f } },
                { 0.88f, { 1.0f, 1.0f, 1.0f, 0.27f, 1.0f, 1.0f } },
                { 1.0f, { 1.0f, 1.0f, 1.0f, 0.0f, 0.7f, 0.7f } }
            };

            block* b = blocks[i];
            for (U32 j = 0; j < type_counts[i]; j++, b++) {
                memset(&b->decal, 0, sizeof(b->decal));
                b->decalCurve.reset(sizeof(xDecal::animix) / sizeof(F32), decalCurve, ARRAY_LENGTH(decalCurve));
                b->decal.flags = xDecal::emit_context::FLAG_ANIM_COLOR | xDecal::emit_context::FLAG_ANIM_UV;
                b->decal.blend_mode = xDecal::BLEND_MODE_AVERAGE;
                b->decal.uv[0].assign(0.0f, 0.0f);
                b->decal.uv[1].assign(1.0f, 1.0f);
                b->decal.texture = decalIDs[i] ? (RwTexture*)xSTFindAsset(decalIDs[i], NULL) : NULL;
                b->decal.curve = &b->decalCurve;
                b->decal.life = 8.0f;
                b->decal.max_dist = 1.0f;
                b->decal.size.assign(decalSizes[i], decalSizes[i]);
            }
        }
    }
}

void zFXFreezeNPC::InitDebugTweak()
{
}

void zFXFreezeNPC::Update(F32 dt)
{
    sFreezeNPC._Update(dt);
}

void zFXFreezeNPC::_Update(F32 dt)
{
    if (frozoneIndex == -1) {
        return;
    }

    for (U32 type = 0; type < MAX_ICE_TYPES; type++) {
        block* b = blocks[type];
        for (U32 i = 0; i < type_counts[type]; i++, b++) {
            if (b->ent) {
                b->emit_remaining += glint_config.emit_rate * dt;
                RpGeometry* geom = b->model->Data->geometry;
                xVec3* verts = (xVec3*)geom->morphTarget->verts;
                while (b->emit_remaining >= 1.0f) {
                    xVec3 pos;
                    pos.Add(verts[xrand_RandomChoice(geom->numVertices)], *(xVec3*)&b->model->Mat->pos);
                    zFX::glint::emit(pos, b->emit_remaining, dt, &glint_config);
                }
            }
        }
    }

    for (U32 i = 0; i < MAX_FREEZING_FX; i++) {
        freezing_fx* fx = &freezing[i];
        if (fx->ent && !xModelIsPreinstanced(fx->ent->model)) {
            fx->emit_remaining += fx->intensity * (fx->intensity * (10.0f * (glint_config.emit_rate * dt)));
            RpGeometry* geom = fx->ent->model->Data->geometry;
            while (fx->emit_remaining >= 1.0f) {
                U32 index = xrand_RandomChoice(geom->numVertices);
                xVec3 pos;
                iModelVertEval(fx->ent->model->Data, index, 1, fx->ent->model->Mat, NULL, &pos);
                zFX::glint::emit(pos, fx->emit_remaining, dt, &glint_config);
            }
        }
    }
}

void zFXFreezeNPC::Freeze(xEnt* ent, F32 amount, U32 type)
{
    sFreezeNPC._Freeze(ent, amount, type);
}

void zFXFreezeNPC::_Freeze(xEnt* ent, F32 amount, U32 type)
{
    if (amount == 0.0f) {
        block* b = find_block(NULL, type);
        if (!b) {
            return;
        }

        b->ent = ent;
        b->amount = 0.0f;

        zPlayer* frozone = CURPLAYERS[frozoneIndex];
        xVec3 dir;
        xVec3Sub(&dir, (xVec3*)&frozone->model->Mat->pos, (xVec3*)&ent->model->Mat->pos);
        xVec3Normalize(&dir, &dir);
        xVec3 at = *(xVec3*)&ent->model->Mat->at;
        F32 angle = xatan2(dir.x, dir.z);
        b->angle = xDangleClamp(angle - xatan2(at.x, at.z));

        b->model->Pipe.Flags = (b->model->Pipe.Flags & ~eCULL_MASK) | ePIPECULL_BACKTHENFRONT;
        RpMatFXMaterialSetEnvMapFrame(b->model->Data->geometry->matList.materials[0], RpAtomicGetFrame(b->ent->model->Data));

        if (b->decal.texture) {
            xMat4x3 mat;
            xVec3Copy(&mat.pos, &ent->frame->mat.pos);
            mat.pos.AddScale(ent->frame->mat.up, 0.5f * b->decal.max_dist);
            xVec3Copy(&mat.up, &ent->frame->mat.at);
            xVec3Copy(&mat.right, &ent->frame->mat.right);
            mat.at.assign(0.0f, -1.0f, 0.0f);
            xDecal::emit(b->decal, mat, k_XENT_COLLTYPE_STAT | k_XENT_COLLTYPE_DYN | k_XENT_COLLTYPE_ENV);
        }
    } else {
        block* b = find_block(ent, type);
        if (b) {
            xprintf("freeze %08X  %6.2f\n", ent, amount);
            b->amount = amount;
        }
    }
}

void zFXFreezeNPC::UpdateAt(xEnt* ent, U32 type)
{
    sFreezeNPC._UpdateAt(ent, type);
}

void zFXFreezeNPC::_UpdateAt(xEnt* ent, U32 type)
{
    block* b = find_block(ent, type);
    if (b) {
        b->angle = 0.0f;
    }
}

void zFXFreezeNPC::Thaw(xEnt* ent, U32 type)
{
    sFreezeNPC._Thaw(ent, type);
}

void zFXFreezeNPC::_Thaw(xEnt* ent, U32 type)
{
    if (frozoneIndex == -1) {
        return;
    }

    block* b = find_block(ent, type);
    if (b && thawShrapnel[type] && thawShrapnel[type]->initCB) {
        for (U32 i = 0; i < 6; i++) {
            thawShrapnel[type]->initCB(thawShrapnel[type], ent->model, NULL, NULL, NULL);
        }
    }

    if (b) {
        b->ent = NULL;
    }

    if (ent) {
        zNPC::zNPCCommon* npc = (zNPC::zNPCCommon*)ent;
        npc->StartRumble((ezNPCRumbleType)(npc->GetGenericRumbleType() - 1), NULL);
    }
}

void zFXFreezeNPC::Release(xEnt* ent, U32 type)
{
    sFreezeNPC._Release(ent, type);
}

void zFXFreezeNPC::_Release(xEnt* ent, U32 type)
{
    if (frozoneIndex == -1) {
        return;
    }

    block* b = find_block(ent, type);
    if (b) {
        b->ent = NULL;
    }
}

void zFXFreezeNPC::Shatter(xEnt* ent, U32 type)
{
    sFreezeNPC._Shatter(ent, type);
}

void zFXFreezeNPC::_Shatter(xEnt* ent, U32 type)
{
    if (frozoneIndex == -1) {
        return;
    }

    block* b = find_block(ent, type);
    if (b && shatterShrapnel[type] && shatterShrapnel[type]->initCB) {
        for (U32 i = 0; i < 4; i++) {
            shatterShrapnel[type]->initCB(shatterShrapnel[type], ent->model, NULL, NULL, NULL);
        }
    }

    if (b) {
        b->ent = NULL;
    }

    if (ent) {
        zNPC::zNPCCommon* npc = (zNPC::zNPCCommon*)ent;
        npc->StartRumble(npc->GetGenericRumbleType(), NULL);
    }
}

void zFXFreezeNPC::Freezing(xEnt* ent, F32 intensity)
{
    sFreezeNPC._Freezing(ent, intensity);
}

void zFXFreezeNPC::_Freezing(xEnt* ent, F32 intensity)
{
    freezing_fx* fx = find_freezing_fx(ent);
    if (!fx) {
        fx = find_freezing_fx(NULL);
        if (fx) {
            fx->ent = ent;
            fx->emit_remaining = 0.0f;
        } else {
            return;
        }
    }
    fx->intensity = intensity;
}

void zFXFreezeNPC::StopFreezing(xEnt* ent)
{
    sFreezeNPC._StopFreezing(ent);
}

void zFXFreezeNPC::_StopFreezing(xEnt* ent)
{
    freezing_fx* fx = find_freezing_fx(ent);
    if (fx) {
        fx->ent = NULL;
    }
}

void zFXFreezeNPC::Render()
{
    sFreezeNPC._Render();
}

void zFXFreezeNPC::_Render()
{
    if (frozoneIndex == -1) {
        return;
    }

    for (U32 type = 0; type < MAX_ICE_TYPES; type++) {
        block* b = blocks[type];
        for (U32 i = 0; i < type_counts[type]; i++, b++) {
            if (b->ent) {
                xVec3 up;
                up = g_Y3;
                xMat4x3* mat = (xMat4x3*)b->model->Mat;
                xVec3Copy(&mat->at, (xVec3*)&b->ent->model->Mat->at);
                xVec3Copy(&mat->up, &up);
                xVec3Cross(&mat->right, &mat->at, &up);
                xMat3x3 rot;
                xMat3x3Rot(&rot, &xVec3::m_UnitAxisY, b->angle);
                xMat3x3Mul(mat, mat, &rot);

                xVec3 scale;
                scale = ice_scale;
                if (type == 1) {
                    scale.y *= b->amount;
                } else {
                    scale.x *= ice_scaling_axes.x ? b->amount * ice_scaling_axes.x : 1.0f;
                    scale.y *= ice_scaling_axes.y ? b->amount * ice_scaling_axes.y : 1.0f;
                    scale.z *= ice_scaling_axes.z ? b->amount * ice_scaling_axes.z : 1.0f;
                }
                xVec3ScaleC(*(xVec3*)&b->model->Mat->right, scale.x);
                xVec3ScaleC(*(xVec3*)&b->model->Mat->up, scale.y);
                xVec3ScaleC(*(xVec3*)&b->model->Mat->at, scale.z);
                xVec3Copy((xVec3*)&b->model->Mat->pos, (xVec3*)&b->ent->model->Mat->pos);

                b->model->Alpha = type == 1 ? (b->amount <= 0.5f ? b->amount / 0.5f : 1.0f) : b->amount;
                xModelRender(b->model);
            }
        }
    }
}
