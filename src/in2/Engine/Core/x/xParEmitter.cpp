#include "xCommon.h"
#include "xParEmitter.h"

#include "xCam.h"
#include "xEnt.h"
#include "xGlobals.h"
#include "xGroup.h"
#include "xMath.h"
#include "xMovePoint.h"
#include "xParEmitterType.h"
#include "xRand.h"
#include "xstransvc.h"
#include "xutil.h"
#include "xVolume.h"
#include "zCam.h"
#include "zEvent.h"
#include "zScene.h"
#include "iMath.h"
#include "iModel.h"

#include <string.h>

#define CONSTA_ID 0x48E48E7A // xStrHash("ConstA")
#define CONSTB_ID 0x48E48E7B // xStrHash("ConstB")
#define SINE_ID 0x0B326F01 // xStrHash("Sine")
#define COSINE_ID 0x498D7119 // xStrHash("Cosine")
#define LINEAR_ID 0xB7353B79 // xStrHash("Linear")
#define STEP_ID 0x0B354BD4 // xStrHash("Step")
#define RANDOM_ID 0x0FE111BF // xStrHash("Random")

#define PROP_INTERP_COUNT 14

static xParEmitterAsset sSaveEmmiterSettings;
static xParEmitterPropsAsset sSaveEmmiterPropSettings;
static xParEmitterPropsAsset sDummyProp;

static void add_tweaks(xParEmitter& emit)
{
}

S32 xParInterpConvertInterpMode(xParInterp* p)
{
    if (p->interp < eParInterpCount)
        return p->interp;

    if (p->interp == CONSTA_ID)
        return eParInterpConstA;
    if (p->interp == CONSTB_ID)
        return eParInterpConstB;
    if (p->interp == SINE_ID)
        return eParInterpSine;
    if (p->interp == COSINE_ID)
        return eParInterpCosine;
    if (p->interp == LINEAR_ID)
        return eParInterpLinear;
    if (p->interp == STEP_ID)
        return eParInterpStep;
    if (p->interp == RANDOM_ID)
        return eParInterpRandom;

    return eParInterpConstA;
}

void xParEmitterInit(void* b, void* tasset)
{
    xParEmitterInit((xBase*)b, (xParEmitterAsset*)tasset);
}

void xParEmitterInit(xBase* b, xParEmitterAsset* pea)
{
    xParEmitterPropsAsset* prop;

    xBaseInit(b, pea);

    xParEmitter* t = (xParEmitter*)b;

    t->eventFunc = xParEmitterEventCB;
    t->tasset = pea;
    t->prop = (xParEmitterPropsAsset*)xSTFindAsset(pea->propID, NULL);
    if (!t->prop) {
        t->prop = &sDummyProp;
    }

    if (t->linkCount)
        t->link = (xLinkAsset*)(t->tasset + 1);
    else
        t->link = NULL;

    prop = t->prop;

    t->emit_flags = pea->emit_flags;

    t->parSys = t->prop->parSysID ? (xParSys*)zSceneFindObject(t->prop->parSysID) : NULL;

    t->attachTo = NULL;

    t->emit_volume = NULL;

    for (S32 i = 0; i < PROP_INTERP_COUNT; i++) {
        if (prop->value[i].val[0] == prop->value[i].val[1])
            prop->value[i].interp = eParInterpConstA;
        else
            prop->value[i].interp = xParInterpConvertInterpMode(&prop->value[i]);
    }

    t->rate_mode = prop->rate.interp;

    if (t->rate_mode == eParInterpRandom) {
        if (prop->rate.val[0] == prop->rate.val[1]) {
            t->rate_mode = eParInterpConstA;
        }
    }

    if (t->rate_mode == eParInterpRandom) {
        prop->rate.order();
    }

    t->rate = t->rate_mode == eParInterpRandom ? (prop->rate.val[1] - prop->rate.val[0]) * xurand() + prop->rate.val[0] : prop->rate.val[0];
    t->rate_time = 0.0f;
    t->rate_fraction = 0.0f;
    t->rate_fraction_cull = 0.0f;
    t->oocull_distance_sqr = (pea->emit_flags & 0x2) ? 1.0f / pea->cull_dist_sqr : 0.0f;

    t->emit_volume = NULL;

    if (pea->emit_type == eParRegionCircle || pea->emit_type == eParRegionCircleEdge) {
        F32 mag2 = pea->e_circle.dir.length2();
        if (mag2 < 0.001f) pea->e_circle.dir.assign(0.0f, 1.0f, 0.0f);
        else pea->e_circle.dir *= 1.0f / xsqrt(mag2);
    }

    t->last_attach_loc = 1e38f;

    t->sortID = 0;
}

void xParEmitterSetup(xParEmitter* t)
{
    if (t->parSys) {
        t->group = t->parSys->group;
    }

    if (t->tasset->attachToID && t->tasset->emit_type == eParRegionOffsetPoint) {
        xEnt* ent = (xEnt*)zSceneFindObject(t->tasset->attachToID);

        iModelTagSetup(&t->tag, ent->model->Data, t->tasset->e_offsetp.offset.x, t->tasset->e_offsetp.offset.y, t->tasset->e_offsetp.offset.z);
    }

    t->attachTo = t->tasset->attachToID ? zSceneFindObject(t->tasset->attachToID) : NULL;

    switch (t->tasset->emit_type) {
    case eParRegionVolume:
        t->emit_volume = zSceneFindObject(t->tasset->e_volume.emit_volumeID);
        break;
    case eParRegionEntBone:
    case eParRegionEntBound:
    default:
        break;
    }

    add_tweaks(*t);
}

void xParEmitterReset(xParEmitter* t)
{
    xBaseReset(t, t->tasset);

    t->emit_flags = t->tasset->emit_flags;
}

void xParEmitterEventCB(xBase*, xBase* to, U32 toEvent, const F32*, xBase*, U32)
{
    xParEmitter* t = (xParEmitter*)to;

    switch (toEvent) {
    case eEventDebugReset:
    case eEventReset:
        xParEmitterReset(t);
        break;
    case eEventOn:
        t->emit_flags |= 0x1;
        break;
    case eEventOff:
        FLAG_REMOVE(t->emit_flags, 0x1);
        break;
    case eEventEmit:
        xParEmitterCustomSettings fake_settings;
        memset(&fake_settings, 0, sizeof(fake_settings));
        xParEmitterEmitCustom(t, 1.0f / 30.0f, &fake_settings);
        break;
    }
}

xPar* xParEmitterEmitCustom(xParEmitter* p, F32 dt, xParEmitterCustomSettings* info)
{
    U32 flags = info->custom_flags;
    xParEmitterAsset* pe = p->tasset;
    if (!pe) {
        return NULL;
    }

    if (flags & 0x1) {
        memcpy(&sSaveEmmiterSettings, pe, sizeof(xParEmitterAsset));
        memcpy(&sSaveEmmiterPropSettings, p->prop, sizeof(xParEmitterPropsAsset));
    }

    if (flags & 0x100) {
        pe->pos = info->pos;
        p->last_emit_position = info->last_emit_position;
    }
    if (flags & 0x10) {
        p->prop->rate = info->rate;
    }
    if (flags & 0x2) {
        p->prop->life = info->life;
    }
    if (flags & 0x2000) {
        p->emit_volume = info->emit_volume;
    }
    if (flags & 0x4) {
        p->prop->size_birth = info->size_birth;
    }
    if (flags & 0x8) {
        p->prop->size_death = info->size_death;
    }

    if (flags & 0x200) {
        pe->vel = info->vel;
        p->last_emit_velocity = info->last_emit_velocity;
        p->prop->vel = info->vel;
    }
    if (flags & 0x40) {
        pe->vel_angle_variation = info->vel_angle_variation;
    }

    if (flags & 0x400) {
        p->prop->color_birth[0] = info->color_birth[0];
        p->prop->color_birth[1] = info->color_birth[1];
        p->prop->color_birth[2] = info->color_birth[2];
        p->prop->color_birth[3] = info->color_birth[3];
    }
    if (flags & 0x800) {
        p->prop->color_death[0] = info->color_death[0];
        p->prop->color_death[1] = info->color_death[1];
        p->prop->color_death[2] = info->color_death[2];
        p->prop->color_death[3] = info->color_death[3];
    }

    if (flags & 0x1000) {
        switch (p->tasset->emit_type) {
        case eParRegionXZCircleEdge:
        case eParRegionXZCircle:
        case eParRegionCircleEdge:
        case eParRegionCircle:
            p->tasset->e_circle.radius = info->radius;
            break;
        case eParRegionSphereEdge:
        case eParRegionSphere:
        case eParRegionSphereEdgeUpper:
        case eParRegionSphereEdgeLower:
            p->tasset->e_sphere.radius = info->radius;
            break;
        case eParRegionPoint:
            break;
        }
    }

    xPar* emitted_par = xParEmitterEmit(p, dt);

    if (flags & 0x1) {
        memcpy(pe, &sSaveEmmiterSettings, sizeof(xParEmitterAsset));
        memcpy(p->prop, &sSaveEmmiterPropSettings, sizeof(xParEmitterPropsAsset));
    }

    return emitted_par;
}

static S32 xParEmitterCull(xParEmitter* pe, xPar* p)
{
    xParEmitterAsset* a = pe->tasset;
    if (a->emit_flags & 0x2) {
        xVec3 camera_pos;
        camera_pos = xglobals->cam->mat.pos;

        xVec3 d;
        xVec3Sub(&d, &camera_pos, &p->m_pos);

        F32 dmag = xVec3Length2(&d);

        pe->distance_to_cull_sqr = dmag;

        if (a->cull_mode == 3)
            return dmag > a->cull_dist_sqr;
        else
            return dmag < a->cull_dist_sqr;
    }
    return FALSE;
}

F32 xParInterpCompute(S32 interp_mode, xParInterp* r, F32 time, S32 time_has_elapsed, F32 lastVal)
{
    F32 val = 0.0f;

    switch (interp_mode) {
    case eParInterpConstA:
        val = r->val[0];
        break;
    case eParInterpConstB:
        val = r->val[1];
        break;
    case eParInterpRandom:
        if (time_has_elapsed) {
            val = (r->val[1] - r->val[0]) * xurand() + r->val[0];
        } else {
            val = lastVal;
        }
        break;
    case eParInterpLinear:
        if (r->freq < 0.00001f) val = r->val[0];
        else val = (r->val[1] - r->val[0]) * (time / r->freq) + r->val[0];
        break;
    case eParInterpSine:
        val = (r->val[1] - r->val[0]) * (0.5f * isin(2.0f * PI * (time * r->oofreq)) + 0.5f) + r->val[0];
        break;
    case eParInterpCosine:
        val = (r->val[1] - r->val[0]) * (0.5f * icos(2.0f * PI * (time * r->oofreq)) + 0.5f) + r->val[0];
        break;
    case eParInterpStep:
        if (time * r->freq >= 0.5f)
            val = r->val[1];
        else
            val = r->val[0];
        break;
    }

    return val;
}

xPar* xParEmitterEmitSetTexIdxs(xPar* p, const xParSys* ps)
{
    xParCmdTex* tex = ps->group->m_cmdTex;
    if (!tex) {
        return NULL;
    }

    switch (tex->birthMode) {
    case 1:
        p->m_texIdx[0] = (xrand_RandomInt32() >> 17) % tex->cols;
        p->m_texIdx[1] = (xrand_RandomInt32() >> 17) % tex->rows;
        break;
    case 0:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        p->m_texIdx[0] = 0;
        p->m_texIdx[1] = 0;
        break;
    }

    return p;
}

xPar* xParEmitterEmit(xParEmitter* pe, F32 emit_dt, F32 par_dt)
{
    if (!pe->parSys) {
        return NULL;
    }

    xPar* last_p = NULL;

    xParEmitterAsset* pea = pe->tasset;
    xParEmitterAsset* a = pea;
    xParEmitterPropsAsset* prop = pe->prop;

    pe->rate_time += emit_dt;
    S32 rate_has_elapsed = pe->rate_time > prop->rate.freq;

    if (prop->rate.freq == 0.0f) {
        pe->rate_time = 0.0f;
    }
    while (pe->rate_time > prop->rate.freq)
        pe->rate_time -= prop->rate.freq;

    F32 rate;
    xParInterp* r = &prop->rate;

    rate = xParInterpCompute(pe->rate_mode, r, pe->rate_time, rate_has_elapsed, pe->rate);

    pe->rate = rate;

    pe->rate_fraction += rate * emit_dt;
    pe->rate_fraction_cull += rate * emit_dt;

    S32 count = std::floorf(pe->rate_fraction);

    if (count > 0) {
        pe->rate_fraction -= count;
    }
    if (count == 0) {
        return NULL;
    }

    xParSys* ps = pe->parSys;
    if (!ps) {
        return NULL;
    }

    xParGroup* g = ps->group;
    if (!g) {
        return NULL;
    }

    if (ps->tasset->maxPar && g->m_num_of_particles >= ps->tasset->maxPar) {
        return NULL;
    }

    if (ps->tasset->maxPar && ps->group->m_num_of_particles >= ps->tasset->maxPar) {
        return NULL;
    }

    xPar* p;
    F32 life = prop->life.val[0], size_birth = prop->size_birth.val[0], size_death = prop->size_death.val[0];
    xVec3 emitPosition = a->pos;

    if (ps->tasset->maxPar && ps->group->m_num_of_particles + count >= ps->tasset->maxPar) {
        count = ps->tasset->maxPar - ps->group->m_num_of_particles;
    }

    xBase* attachObject = (xBase*)pe->attachTo;
    U32 attachGroupIndex = 0xFFFFFFFF, attachGroupTotal = 0xFFFFFFFF;
    S32 emitAgain, emitterIsAttachedToObject = FALSE;

    do {
        emitAgain = FALSE;
        xBase* emitObj = NULL;
        S32 marker = FALSE;

        if (attachObject) {
            if (attachObject->baseType == eBaseTypeGroup) {
                xGroup* g = (xGroup*)attachObject;

                if (attachGroupIndex == 0xFFFFFFFF) {
                    attachGroupTotal = xGroupGetCount(g);
                    attachGroupIndex = 0;
                }

                if (g->asset->groupFlags & 0x1) {
                    U32 get_rnd_group_idx = xrand_RandomChoice(attachGroupTotal);

                    emitObj = xGroupGetItemPtr(g, get_rnd_group_idx);
                    emitAgain = FALSE;

                    if (!emitObj) {
                        emitObj = (xBase*)xSTFindAsset(xGroupGetItem(g, get_rnd_group_idx), NULL);
                        if (emitObj)
                            marker = TRUE;
                    }
                } else {
                    emitObj = xGroupGetItemPtr(g, attachGroupIndex);
                    if (!emitObj) {
                        emitObj = (xBase*)xSTFindAsset(xGroupGetItem(g, attachGroupIndex), NULL);
                        if (emitObj) {
                            marker = TRUE;
                        }
                    }

                    attachGroupIndex++;
                    emitAgain = attachGroupIndex < attachGroupTotal ? TRUE : FALSE;
                }
            } else {
                emitObj = attachObject;
            }
        }

        const xEnt* attach_ent = (const xEnt*)emitObj;
        if (emitObj) {
            emitterIsAttachedToObject = TRUE;
            if (marker) {
                emitPosition = *(xVec3*)emitObj;
                attach_ent = NULL;
            } else {
                switch (emitObj->baseType) {
                case eBaseTypeMovePoint:
                    emitPosition = *((xMovePoint*)emitObj)->pos;
                    attach_ent = NULL;
                    break;
                default:
                    if (!xEntValidType(attach_ent->baseType) || !attach_ent->model) {
                        attach_ent = NULL; emitPosition = 0.0f;
                    } else {
                        emitPosition = *xEntGetPos(attach_ent);
                    }
                }
            }
        } else {
            emitPosition = a->pos;
        }

        if ((pe->emit_flags & 0x10) && attach_ent && !xEntIsVisible(attach_ent)) {
            continue;
        }

        const xMat4x3* bone_mat = NULL;
        xVec3 bone_vel;
        if (a->emit_type == eParRegionEntBone && attach_ent) {
            bone_mat = xParEmitterTransformEntBone(emitPosition, bone_vel, *a, *attach_ent);
            bone_vel *= par_dt;
        }

        xVec3 add_vel;
        if (pe->emit_flags & 0x8) {
            add_vel.Sub(emitPosition, pe->last_attach_loc);
            pe->last_attach_loc = emitPosition;

            if (add_vel.length2() > 25.0f)
                add_vel = 0.0f;
        } else {
            add_vel = 0.0f;
        }

        F32 last_emit_time = -emit_dt;

        const xVec3& camPos = zCamGetMatrix().pos;

        for (S32 i = 0; i < count; i++) {
            xParGroupSetSortDistance(camPos.Distance2(emitPosition), pe->sortID);

            p = xParGroupAddPar(ps->group);
            if (p) {
                last_p = p;

                life = xParInterpCompute(prop->life.interp, &prop->life, pe->rate_time, TRUE, 0.0f);

                size_birth = xParInterpCompute(prop->size_birth.interp, &prop->size_birth, pe->rate_time, TRUE, 0.0f);
                size_death = xParInterpCompute(prop->size_death.interp, &prop->size_death, pe->rate_time, TRUE, 0.0f);

                p->m_lifetime = life;
                p->totalLifespan = life;

                p->m_size = size_birth;
                if (size_death == size_birth)
                    p->m_sizeVel = 0.0f;
                else
                    p->m_sizeVel = (size_death - size_birth) / life;

                p->m_flag = 0;

                p->m_rotdeg[0] = pe->rot[0];
                p->m_rotdeg[1] = pe->rot[1];
                p->m_rotdeg[2] = pe->rot[2];

                for (S32 c = 0; c < (S32)ARRAY_LENGTH(prop->color_birth); c++) {
                    F32 fc1 = xParInterpCompute(prop->color_birth[c].interp, &prop->color_birth[c], pe->rate_time, TRUE, 0.0f);
                    F32 fc2 = xParInterpCompute(prop->color_death[c].interp, &prop->color_death[c], pe->rate_time, TRUE, 0.0f);

                    p->m_cfl[c] = fc1;
                    p->m_c[c] = fc1;
                    p->m_cvel[c] = (fc2 - fc1) / life;
                }

                p->m_pos = emitPosition;

                xParEmitterEmitSetTexIdxs(p, ps);

                switch (a->emit_type) {
                case eParRegionPoint:
                    xParEmitterEmitPoint(p, a, par_dt);
                    break;
                case eParRegionXZCircleEdge:
                    xParEmitterEmitCircleEdge(p, a, par_dt);
                    break;
                case eParRegionXZCircle:
                    xParEmitterEmitCircle(p, a, par_dt);
                    break;
                case eParRegionXZRectEdge:
                    xParEmitterEmitRectEdge(p, a, par_dt);
                    break;
                case eParRegionXZRect:
                    xParEmitterEmitRect(p, a, par_dt);
                    break;
                case eParRegionLine:
                    xParEmitterEmitLine(p, a, par_dt);
                    break;
                case eParRegionSphereEdge:
                    xParEmitterEmitSphereEdge(p, a, par_dt, eParRegionSphereEdge);
                    break;
                case eParRegionSphereEdgeUpper:
                    xParEmitterEmitSphereEdge(p, a, par_dt, eParRegionSphereEdgeUpper);
                    break;
                case eParRegionSphereEdgeLower:
                    xParEmitterEmitSphereEdge(p, a, par_dt, eParRegionSphereEdgeLower);
                    break;
                case eParRegionSphere:
                    xParEmitterEmitSphere(p, a, par_dt);
                    break;
                case eParRegionVolume:
                {
                    xBase* obj = pe->emit_volume;
                    if (obj) {
                        if (obj->baseType == eBaseTypeVolume)
                            xParEmitterEmitVolume(p, a, par_dt, (xVolume*)obj);
                        else {
                            xParEmitterEmitEntity(p, a, par_dt, (xEnt*)obj);
                        }
                    }
                    break;
                }
                case eParRegionOffsetPoint:
                    xParEmitterEmitOffsetPoint(pe, p, a, par_dt, (xEnt*)emitObj);
                    break;
                case eParRegionVCylEdge:
                    xParEmitterEmitVCylEdge(p, a, par_dt);
                    break;
                case eParRegionCircleEdge:
                    xParEmitterEmitOCircleEdge(p, a, par_dt);
                    break;
                case eParRegionCircle:
                    xParEmitterEmitOCircle(p, a, par_dt);
                    break;
                case eParRegionEntBone:
                    if (bone_mat) {
                        p->m_vel = bone_vel;
                        xParEmitterEmitEntBone(p, a, par_dt, *bone_mat);
                    }
                    break;
                case eParRegionEntBound:
                    if (attach_ent) {
                        xParEmitterEmitEntBound(p, a, par_dt, attach_ent);
                    }
                    break;
                default:
                    break;
                }

                last_emit_time += emit_dt / count;

                bool tempGroupInitialized = false;
                static xParGroup temp_group;

                if (!tempGroupInitialized) {
                    tempGroupInitialized = true;
                    xParGroupInit(&temp_group);
                    pe->last_emit_position = p->m_pos;
                    pe->last_emit_velocity = p->m_vel;
                }

                temp_group.m_root = NULL;

                xPar* save_next = p->m_next, *save_prev = p->m_prev;
                xParGroupAddParP(&temp_group, p);

                F32 u = -last_emit_time / emit_dt;

                xVec3 pos_diff;
                if (emitterIsAttachedToObject) {
                    pos_diff.Sub(p->m_pos, emitPosition);
                    p->m_pos.Lerp(emitPosition, pe->last_emit_position, u);
                    p->m_pos.Add(pos_diff);
                } else {
                    pos_diff.Sub(p->m_pos, a->pos);
                    p->m_pos.Lerp(a->pos, pe->last_emit_position, u);
                    p->m_pos.Add(pos_diff);
                }

                F32 initial_length = p->m_vel.length();

                p->m_vel.Lerp(p->m_vel, pe->last_emit_velocity, u);

                if (initial_length > 0.00001f)
                    p->m_vel /= initial_length;
                p->m_vel *= initial_length * (1.0f - u) + u * pe->last_emit_velocity.length();

                p->m_lifetime += last_emit_time;

                if (p->m_lifetime < 0.0f) {
                    p->m_next = save_next;
                    p->m_prev = save_prev;
                    xParGroupKillPar(ps->group, p);
                    continue;
                }

                xParGroupUpdate(ps, &temp_group, -last_emit_time);

                p->m_next = save_next;
                p->m_prev = save_prev;

                p->m_vel += add_vel;

                if (xParEmitterCull(pe, p)) {
                    xParGroupKillPar(ps->group, p);
                }
            } else {
                emitAgain = FALSE;
            }
        }
    } while (emitAgain);

    pe->last_emit_position = emitPosition;
    pe->last_emit_velocity = a->vel;

    return last_p;
}

void xParEmitterUpdate(xBase* to, xScene*, F32 dt)
{
    xParEmitter* pe = (xParEmitter*)to;

    if (pe->parSys) {
        if (pe->emit_flags & 0x1)
            xParEmitterEmit(pe, dt);
    }
}
