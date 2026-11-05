#include "xParCmd.h"

#include "xMath.h"
#include "xMath3.h"
#include "xParGroup.h"
#include "xRand.h"

struct xCmdInfo
{
    U32 type;
    U32 size;
    xParCmdUpdateFunction func;
};

static xCmdInfo sCmdInfo[eParCmdCount] = { 0 };

void xParCmdInit()
{
    xParCmdRegister(eParCmdMove, sizeof(xParCmdMove), xParCmdMove_Update);
    xParCmdRegister(eParCmdMoveRandom, sizeof(xParCmdMoveRandom), xParCmdMoveRandom_Update);
    xParCmdRegister(eParCmdAccelerate, sizeof(xParCmdAccelerate), xParCmdAccelerate_Update);
    xParCmdRegister(eParCmdVelocityApply, sizeof(xParCmdVelocityApply), xParCmdVelocityApply_Update);
    xParCmdRegister(eParCmdKillOld, sizeof(xParCmdKillOld), NULL);
    xParCmdRegister(eParCmdKillSlow, sizeof(xParCmdKillSlow), xParCmdKillSlow_Update);
    xParCmdRegister(eParCmdFollow, sizeof(xParCmdFollow), xParCmdFollow_Update);
    xParCmdRegister(eParCmdOrbitPoint, sizeof(xParCmdOrbitPoint), xParCmdOrbitPoint_Update);
    xParCmdRegister(eParCmdOrbitLine, sizeof(xParCmdOrbitLine), xParCmdOrbitLine_Update);
    xParCmdRegister(eParCmdMoveRandomPar, sizeof(xParCmdMoveRandomPar), xParCmdMoveRandomPar_Update);
    xParCmdRegister(eParCmdScale3rdPolyReg, sizeof(xParCmdScale3rdPolyReg), xParCmdScale3rdPolyReg_Update);
    xParCmdRegister(eParCmdTex, sizeof(xParCmdTex), xParCmdTex_Update);
    xParCmdRegister(eParCmdTexAnim, sizeof(xParCmdTexAnim), xParCmdTexAnim_Update);
    xParCmdRegister(eParCmdRandomVelocityPar, sizeof(xParCmdRandomVelocityPar), xParCmdRandomVelocityPar_Update);
    xParCmdRegister(eParCmdAge, sizeof(xParCmdAge), xParCmdAge_Update);
    xParCmdRegister(eParCmdAlpha3rdPolyReg, sizeof(xParCmdAlpha3rdPolyReg), xParCmdAlpha3rdPolyReg_Update);
    xParCmdRegister(eParCmdApplyWind, sizeof(xParCmdApplyWind), xParCmdApplyWind_Update);
    xParCmdRegister(eParCmdRotPar, sizeof(xParCmdRotPar), xParCmdRotPar_Update);
    xParCmdRegister(eParCmdRotateAround, sizeof(xParCmdRotateAround), xParCmdRotateAround_Update);
    xParCmdRegister(eParCmdSmokeAlpha, sizeof(xParCmdSmokeAlpha), xParCmdSmokeAlpha_Update);
    xParCmdRegister(eParCmdScale, sizeof(xParCmdScale), xParCmdScale_Update);
    xParCmdRegister(eParCmdCollideFall, sizeof(xParCmdCollideFall), xParCmdCollideFall_Update);
    xParCmdRegister(eParCmdCollideFallSticky, sizeof(xParCmdCollideFallSticky), xParCmdCollideFallSticky_Update);

    xParCmdRegister(eParCmdShaper, sizeof(xParCmdShaperData), xParCmd_Shaper_Update);
    xParCmdRegister(eParCmdAlphaInOut, sizeof(xParCmdAlphaInOutData), xParCmd_AlphaInOut_Update);
    xParCmdRegister(eParCmdSizeInOut, sizeof(xParCmdSizeInOutData), xParCmd_SizeInOut_Update);
    xParCmdRegister(eParCmdDampenSpeed, sizeof(xParCmdDampenData), xParCmd_DampenSpeed_Update);
}

void xParCmdRegister(U32 parType, U32 size, xParCmdUpdateFunction func)
{
    sCmdInfo[parType].type = parType;
    sCmdInfo[parType].size = size;
    sCmdInfo[parType].func = func;
}

U32 xParCmdGetSize(U32 parType)
{
    for (S32 i = 0; i < eParCmdCount; i++) {
        if (parType == sCmdInfo[i].type)
            return sCmdInfo[i].size;
    }

    return 0;
}

xParCmdUpdateFunction xParCmdGetUpdateFunc(U32 parType)
{
    for (S32 i = 0; i < eParCmdCount; i++) {
        if (parType == sCmdInfo[i].type)
            return sCmdInfo[i].func;
    }

    return NULL;
}

void xParCmdKillSlow_Update(xParCmd* c, xParGroup* ps, F32 dt)
{
    xPar* p;
    xParCmdKillSlow* cmd = (xParCmdKillSlow*)c->tasset;

    F32 speedLimit = cmd->speedLimitSqr * dt;
    p = ps->m_root;

    if (cmd->kill_less_than) {
        while (p) {
            if (xVec3Length2(&p->m_vel) < speedLimit) {
                p->m_lifetime = -1.0f;
            }
            p = p->m_next;
        }
    } else {
        while (p) {
            if (xVec3Length2(&p->m_vel) > speedLimit) {
                p->m_lifetime = -1.0f;
            }
            p = p->m_next;
        }
    }
}

void xParCmdAge_Update(xParCmd* c, xParGroup* ps, F32 dt)
{
    xPar* p;
    xParCmdAge* cmd = (xParCmdAge*)c->tasset;

    F32 age_rate = cmd->age_rate * dt;

    p = ps->m_root;
    while (p) {
        p->m_lifetime -= age_rate;
        p = p->m_next;
    }
}

void xParCmdFollow_Update(xParCmd* c, xParGroup* ps, F32 dt)
{
    xPar* p;
    xParCmdFollow* cmd = (xParCmdFollow*)c->tasset;
    xVec3 delta;

    F32 mdt = cmd->gravity * dt;

    p = ps->m_root;
    while (p && p->m_next) {
        xVec3Sub(&delta, &p->m_next->m_pos, &p->m_pos);

        F32 lenSqr2 = xVec3Length2(&delta);
        F32 lenSqr = xVec3LengthFast(delta.x, delta.y, delta.z);

        F32 force = mdt / (lenSqr * (lenSqr2 + cmd->epsilon));

        p->m_vel.x += delta.x * force;
        p->m_vel.y += delta.y * force;
        p->m_vel.z += delta.z * force;

        p = p->m_next;
    }
}

void xParCmdOrbitPoint_Update(xParCmd* c, xParGroup* ps, F32 dt)
{
    xPar* p;
    xParCmdOrbitPoint* cmd = (xParCmdOrbitPoint*)c->tasset;
    xVec3 dir;

    F32 mdt = cmd->gravity * dt;

    p = ps->m_root;
    while (p) {
        xVec3Sub(&dir, &cmd->center, &p->m_pos);

        F32 rSqr2 = xVec3Length2(&dir);

        if (rSqr2 < cmd->maxRadiusSqr) {
            F32 rSqr = xVec3LengthFast(dir.x, dir.y, dir.z);

            F32 force = mdt / (rSqr + (rSqr2 + cmd->epsilon));
            p->m_vel.x += dir.x * force;
            p->m_vel.y += dir.y * force;
            p->m_vel.z += dir.z * force;
        }

        p = p->m_next;
    }
}

void xParCmdOrbitLine_Update(xParCmd* c, xParGroup* ps, F32 dt)
{
    xPar* p;
    xParCmdOrbitLine* cmd = (xParCmdOrbitLine*)c->tasset;
    xVec3 f, w, tmp;

    F32 mdt = cmd->gravity * dt;

    p = ps->m_root;
    while (p) {
        xVec3Sub(&f, &p->m_pos, &cmd->p);

        xVec3Cross(&tmp, &f, &cmd->axis);
        xVec3Cross(&w, &cmd->axis, &tmp);

        xVec3 into;
        xVec3Sub(&into, &w, &f);

        F32 rSqr2 = xVec3Length2(&into);

        if (rSqr2 < cmd->maxRadiusSqr) {
            F32 rSqr = xVec3LengthFast(into.x, into.y, into.z);

            F32 force = mdt / (rSqr + (rSqr2 + cmd->epsilon));
            p->m_vel.x += into.x * force;
            p->m_vel.y += into.y * force;
            p->m_vel.z += into.z * force;
        }

        p = p->m_next;
    }
}

void xParCmdAccelerate_Update(xParCmd* c, xParGroup* ps, F32 dt)
{
    xPar* p;
    xParCmdAccelerate* cmd = (xParCmdAccelerate*)c->tasset;
    xVec3 delta;

    delta = cmd->acc;
    xVec3SMulBy(&delta, dt);

    p = ps->m_root;
    while (p) {
        p->m_vel.x += delta.x;
        p->m_vel.y += delta.y;
        p->m_vel.z += delta.z;
        p = p->m_next;
    }
}

void xParCmdMove_Update(xParCmd* c, xParGroup* ps, F32 dt)
{
    xPar* p;
    xParCmdMove* cmd = (xParCmdMove*)c->tasset;
    xVec3 delta;

    delta = cmd->dir;
    xVec3SMulBy(&delta, dt);

    p = ps->m_root;
    while (p) {
        xVec3Add(&p->m_pos, &p->m_pos, &delta);
        p = p->m_next;
    }
}

void xParCmdMoveRandom_Update(xParCmd* c, xParGroup* ps, F32 dt)
{
    xPar* p;
    xParCmdMove* cmd = (xParCmdMove*)c->tasset;
    xVec3 delta;

    delta = cmd->dir;
    delta.x *= 2.0f * xurand();
    delta.y *= 2.0f * xurand();
    delta.z *= 2.0f * xurand();
    xVec3Sub(&delta, &delta, &cmd->dir);
    xVec3SMulBy(&delta, dt);

    p = ps->m_root;
    while (p) {
        xVec3Add(&p->m_pos, &p->m_pos, &delta);
        p = p->m_next;
    }
}

void xParCmdMoveRandomPar_Update(xParCmd* c, xParGroup* ps, F32 dt)
{
    xPar* p;
    xParCmdMoveRandomPar* cmd = (xParCmdMoveRandomPar*)c->tasset;
    xVec3 delta;

    dt *= 0.5f;
    delta.x = cmd->dim.x * dt;
    delta.y = cmd->dim.y * dt;
    delta.z = cmd->dim.z * dt;

    p = ps->m_root;
    while (p) {
        p->m_pos.x += delta.x * (xurand() - 0.5f);
        p->m_pos.z += delta.z * (xurand() - 0.5f);
        p = p->m_next;
    }
}

void xParCmdScale3rdPolyReg_Update(xParCmd*, xParGroup*, F32)
{
}

void xParCmdSmokeAlpha_Update(xParCmd*, xParGroup*, F32)
{
}

void xParCmdScale_Update(xParCmd*, xParGroup*, F32)
{
}

void xParCmdAlpha3rdPolyReg_Update(xParCmd*, xParGroup*, F32)
{
}

void xParCmdRandomVelocityPar_Update(xParCmd* c, xParGroup* ps, F32 dt)
{
    xPar* p;
    xParCmdRandomVelocityPar* cmd = (xParCmdRandomVelocityPar*)c->tasset;

    xVec3 rot;
    rot.x = cmd->x * dt;
    rot.y = cmd->y * dt;
    rot.z = cmd->z * dt;

    xMat3x3 rotmat;

    p = ps->m_root;
    while (p) {
        F32 x = 2.0f * (rot.x * xurand()) - rot.x;
        F32 y = 2.0f * (rot.y * xurand()) - rot.y;
        F32 z = 2.0f * (rot.z * xurand()) - rot.z;

        xMat3x3Euler(&rotmat, y, x, z);

        xMat3x3LMulVec(&p->m_vel, &rotmat, &p->m_vel);

        p = p->m_next;
    }
}

void xParCmdApplyWind_Update(xParCmd* c, xParGroup* ps, F32 dt)
{
    xPar* p;
    xParCmdApplyWind* cmd = (xParCmdApplyWind*)c->tasset;

    xVec3 wind_vec;
    wind_vec.x = 1.0f;
    wind_vec.y = 0.0f;
    wind_vec.z = 1.0f;

    wind_vec.x *= cmd->strength * dt;
    wind_vec.z *= cmd->strength * dt;

    p = ps->m_root;
    while (p) {
        p->m_vel.x += wind_vec.x;
        p->m_vel.z += wind_vec.z;
        p = p->m_next;
    }
}

void xParCmdRotPar_Update(xParCmd* c, xParGroup* ps, F32 dt)
{
    xPar* p;
    xParCmdRotPar* cmd = (xParCmdRotPar*)c->tasset;

    xVec3 range;
    range.x = 255.0f * ((cmd->max.x - cmd->min.x) / 360.0f);
    range.y = 255.0f * ((cmd->max.y - cmd->min.y) / 360.0f);
    range.z = 255.0f * ((cmd->max.z - cmd->min.z) / 360.0f);

    xVec3 min;
    min.x = 255.0f * (cmd->min.x / 360.0f);
    min.y = 255.0f * (cmd->min.y / 360.0f);
    min.z = 255.0f * (cmd->min.z / 360.0f);

    p = ps->m_root;
    while (p) {
        p->m_rotdeg[0] += dt * (range.x * xurand() + min.x);
        p->m_rotdeg[1] += dt * (range.y * xurand() + min.y);
        p->m_rotdeg[2] += dt * (range.z * xurand() + min.z);

        p = p->m_next;
    }
}

void xParCmdVelocityApply_Update(xParCmd* c, xParGroup* ps, F32 dt)
{
    xPar* p;

    xVec3 vel;

    p = ps->m_root;
    while (p) {
        vel.Scale(p->m_vel, dt);
        xVec3Add(&p->m_pos, &p->m_pos, &vel);

        p = p->m_next;
    }
}

void xParCmdRotateAround_Update(xParCmd* c, xParGroup* ps, F32 dt)
{
    xPar* p;
    xParCmdRotateAround* cmd = (xParCmdRotateAround*)c->tasset;

    F32 yaw = dt * cmd->yaw;
    yaw = (PI / 180.0f) * yaw;
    F32 radius_growth = dt * cmd->radius_growth;

    p = ps->m_root;
    while (p) {
        xVec3 at;
        xVec3Sub(&at, &cmd->pos, &p->m_pos);
        at.y = 0.0f;

        xMat3x3 lookmat;
        F32 radius = xMat3x3LookVec(&lookmat, &at);

        xVec3 angles;
        xMat3x3GetEuler(&lookmat, &angles);

        angles.x += yaw;

        xMat3x3 rotmat;
        xMat3x3Euler(&rotmat, angles.x, angles.y, angles.z);

        radius += radius_growth;

        xVec3 offset, out_offset;
        offset.x = 0.0f;
        offset.y = 0.0f;
        offset.z = radius;

        xMat3x3RMulVec(&out_offset, &rotmat, &offset);

        p->m_pos.x = out_offset.x + cmd->pos.x;
        p->m_pos.z = out_offset.z + cmd->pos.z;

        p = p->m_next;
    }
}

void xParCmdTex_Update(xParCmd* c, xParGroup* ps, F32)
{
}

#define TEXIDX_NEXT(idx, count) idx++; if (idx >= count) idx = 0
#define TEXIDX_PREV(idx, count) if (idx == 0) idx = count - 1; else idx--

void xParCmdTexAnim_Update(xParCmd* c, xParGroup* ps, F32 dt)
{
    xPar* p;
    xParCmdTexAnim* cmd = (xParCmdTexAnim*)c->tasset;

    xParCmdTex* tex = ps->m_cmdTex;
    if (!tex) {
        return;
    }

    if (cmd->throttle_time > 0.0f) {
        cmd->throttle_time_elapsed -= dt;
        if (cmd->throttle_time_elapsed > 0.0f) {
            return;
        }
        cmd->throttle_time_elapsed = cmd->throttle_time;
    }

    p = ps->m_root;

    if (cmd->anim_mode == 0) {
        while (p) {
            p->m_texIdx[0] = xrand_RandomChoice(tex->cols);
            p->m_texIdx[1] = xrand_RandomChoice(tex->rows);
            p = p->m_next;
        }
    }
    else if (cmd->anim_mode == 1) {
        if (tex->cols > 1) {
            while (p) {
                p->m_texIdx[0]++;
                if (p->m_texIdx[0] >= tex->cols) {
                    p->m_texIdx[0] = 0;
                    if (cmd->anim_wrap_mode == 1) {
                        TEXIDX_NEXT(p->m_texIdx[1], tex->rows);
                    }
                    else if (cmd->anim_wrap_mode == 2) {
                        TEXIDX_PREV(p->m_texIdx[1], tex->rows);
                    }
                    else if (cmd->anim_wrap_mode == 3) {
                        p->m_texIdx[1]++;
                        if (p->m_texIdx[1] >= tex->rows) {
                            p->m_texIdx[1] = tex->rows - 1;
                            p->m_texIdx[0] = tex->cols - 1;
                        }
                    }
                }
                p = p->m_next;
            }
        }
    }
    else if (cmd->anim_mode == 2) {
        if (tex->cols > 1) {
            while (p) {
                if (p->m_texIdx[0] == 0) {
                    p->m_texIdx[0] = tex->cols - 1;
                    if (cmd->anim_wrap_mode == 1) {
                        TEXIDX_NEXT(p->m_texIdx[1], tex->rows);
                    }
                    else if (cmd->anim_wrap_mode == 2) {
                        TEXIDX_PREV(p->m_texIdx[1], tex->rows);
                    }
                } else {
                    p->m_texIdx[0]--;
                }
                p = p->m_next;
            }
        }
    }
    else if (cmd->anim_mode == 3) {
        if (tex->rows > 1) {
            while (p) {
                if (p->m_texIdx[1] == 0) {
                    p->m_texIdx[1] = tex->rows - 1;
                    if (cmd->anim_wrap_mode == 1) {
                        TEXIDX_NEXT(p->m_texIdx[0], tex->cols);
                    }
                    else if (cmd->anim_wrap_mode == 2) {
                        TEXIDX_PREV(p->m_texIdx[0], tex->cols);
                    }
                } else {
                    p->m_texIdx[1]--;
                }
                p = p->m_next;
            }
        }
    }
    else if (cmd->anim_mode == 4) {
        if (tex->rows > 1) {
            while (p) {
                p->m_texIdx[1]++;
                if (p->m_texIdx[1] >= tex->rows) {
                    p->m_texIdx[1] = 0;
                    if (cmd->anim_wrap_mode == 1) {
                        TEXIDX_NEXT(p->m_texIdx[0], tex->cols);
                    }
                    else if (cmd->anim_wrap_mode == 2) {
                        TEXIDX_PREV(p->m_texIdx[0], tex->cols);
                    }
                }
                p = p->m_next;
            }
        }
    }
}

void xParCmdCollideFall_Update(xParCmd* c, xParGroup* ps, F32)
{
    xParCmdCollideFall& cmd = *(xParCmdCollideFall*)c->tasset;

    for (xPar* p = ps->m_root; p; p = p->m_next) {
        F32& loc = p->m_pos.y;
        F32& vel = p->m_vel.y;
        F32 dloc = cmd.y - loc;
        if (dloc < 0.0f) continue;
        loc = dloc * cmd.bounce + cmd.y;
        if (vel < 0.0f) vel = -vel * cmd.bounce;
    }
}

void xParCmdCollideFallSticky_Update(xParCmd* c, xParGroup* ps, F32)
{
    xParCmdCollideFallSticky& cmd = *(xParCmdCollideFallSticky*)c->tasset;
    F32 xzdamp = 1.0f - cmd.sticky;

    for (xPar* p = ps->m_root; p; p = p->m_next) {
        F32& loc = p->m_pos.y;
        F32& vel = p->m_vel.y;
        F32 dloc = cmd.y - loc;
        if (dloc < 0.0f) continue;
        loc = dloc * cmd.bounce + cmd.y;
        if (vel < 0.0f) vel = -vel * cmd.bounce;
        p->m_vel.x *= xzdamp;
        p->m_vel.z *= xzdamp;
    }
}

void xParCmd_DampenSpeed_Update(xParCmd* c, xParGroup* ps, F32 dt)
{
    xPar* p;
    xParCmdDampenData* cmd = (xParCmdDampenData*)c->tasset;

    if (cmd->enabled) {
        F32 damp = dt * cmd->dampSpeed;

        p = ps->m_root;
        while (p) {
            p->m_vel.AddScale(p->m_vel, damp);
            p = p->m_next;
        }
    }
}

static const S32 NUM_SEGS = 3;

void xParCmd_SizeInOut_Update(xParCmd* c, xParGroup* ps, F32)
{
    xPar* p;
    xParCmdSizeInOutData* cmd = (xParCmdSizeInOutData*)c->tasset;

    if (cmd->enabled) {
        S32 i = 0, seg;
        F32 slope_size[NUM_SEGS];

        for (i = 0; i < NUM_SEGS; i++) {
            slope_size[i] = 3.0f * (cmd->custSize[i + 1] - cmd->custSize[i]);
        }

        p = ps->m_root;
        while (p) {
            F32 frac = 1.0f - p->m_lifetime / p->totalLifespan;
            frac = CLAMP(frac, 0.0f, 1.0f);

            if (frac < 1.0f / 3.0f) seg = 0;
            else if (frac < 2.0f / 3.0f) seg = 1;
            else seg = 2;

            i = seg;
            while (i-- > 0) frac -= 1.0f / 3.0f;

            p->m_size = frac * slope_size[seg] + cmd->custSize[seg];

            p = p->m_next;
        }
    }
}

void xParCmd_AlphaInOut_Update(xParCmd* c, xParGroup* ps, F32)
{
    xPar* p;
    xParCmdAlphaInOutData* cmd = (xParCmdAlphaInOutData*)c->tasset;

    if (cmd->enabled) {
        S32 i = 0, seg;
        F32 slope_alfa[NUM_SEGS];

        for (i = 0; i < NUM_SEGS; i++) {
            slope_alfa[i] = 3.0f * (cmd->custAlpha[i + 1] - cmd->custAlpha[i]);
        }

        p = ps->m_root;
        while (p) {
            F32 frac = 1.0f - p->m_lifetime / p->totalLifespan;
            frac = CLAMP(frac, 0.0f, 1.0f);

            if (frac < 1.0f / 3.0f) seg = 0;
            else if (frac < 2.0f / 3.0f) seg = 1;
            else seg = 2;

            i = seg;
            while (i-- > 0) frac -= 1.0f / 3.0f;

            F32 alfa = frac * slope_alfa[seg] + cmd->custAlpha[seg];
            p->m_cfl[3] = CLAMP(alfa, 0.0f, 255.0f);
            p->m_c[3] = p->m_cfl[3];

            p = p->m_next;
        }
    }
}

void xParCmd_Shaper_Update(xParCmd* c, xParGroup* ps, F32 dt)
{
    xPar* p;
    xParCmdShaperData* cmd = (xParCmdShaperData*)c->tasset;

    if (cmd->enabled) {
        F32 damp = dt * cmd->dampSpeed;
        F32 grav = dt * cmd->gravity;

        S32 doalpha = TRUE;
        S32 dosize = TRUE;
        if (cmd->custAlpha[0] < 0.0f) doalpha = FALSE;
        if (cmd->custSize[0] < 0.0f) dosize = FALSE;

        S32 i, seg;
        F32 slope_alfa[NUM_SEGS], slope_size[NUM_SEGS];

        for (i = 0; i < NUM_SEGS; i++) {
            slope_size[i] = 3.0f * (cmd->custSize[i + 1] - cmd->custSize[i]);
            slope_alfa[i] = 3.0f * (cmd->custAlpha[i + 1] - cmd->custAlpha[i]);
        }

        p = ps->m_root;
        while (p) {
            p->m_vel.AddScale(p->m_vel, damp);
            p->m_vel.y -= grav;

            if (p->totalLifespan < EPSILON || (!dosize && !doalpha)) {
                p = p->m_next;
                continue;
            }

            F32 frac = 1.0f - p->m_lifetime / p->totalLifespan;
            frac = CLAMP(frac, 0.0f, 1.0f);

            if (frac < 1.0f / 3.0f) seg = 0;
            else if (frac < 2.0f / 3.0f) seg = 1;
            else seg = 2;

            i = seg;
            while (i-- > 0) frac -= 1.0f / 3.0f;

            if (dosize) {
                p->m_size = frac * slope_size[seg] + cmd->custSize[seg];
            }

            if (doalpha) {
                F32 alfa = frac * slope_alfa[seg] + cmd->custAlpha[seg];
                p->m_cfl[3] = CLAMP(alfa, 0.0f, 255.0f);
                p->m_c[3] = p->m_cfl[3];
            }

            p = p->m_next;
        }
    }
}
