#include "xCommon.h"
#include "xCam.h"

#include "iCamera.h"
#include "xCamTransition.h"
#include "xDebugTweak.h"
#include "xMemMgr.h"
#include "xScene.h"
#include "xutil.h"
#include "zMain.h"
#include "zPad.h"

static void do_transient_replacement(xCam*& curCam);

namespace {
    namespace tweak {
        void add_tweaks(const char* prefix)
        {
        }
    }
}

void xCam::start()
{
    flags |= 0x1;
}

void xCam::stop()
{
    flags &= ~0x1;
}

void xCam::scene_enter()
{
    tweak::add_tweaks("Camera|\001Globals|");
}

void xCam::scene_exit()
{
    xTWEAK_REMOVE("Camera");
}

void xCam::refresh_mat(xMat4x3& mat, const xCamSpatialInfo& spatial)
{
    switch (coord_type) {
    case XCAM_COORD_CART:
        coord_to_world(mat.pos, spatial.coord.cart);
        break;
    case XCAM_COORD_CYLINDER:
        coord_to_world(mat.pos, spatial.coord.cylinder);
        break;
    case XCAM_COORD_SPHERE:
        coord_to_world(mat.pos, spatial.coord.sphere);
        break;
    }

    switch (orient_type) {
    case XCAM_ORIENT_QUAT:
        orient_to_world(mat, spatial.orient.quat);
        break;
    case XCAM_ORIENT_EULER:
        orient_to_world(mat, spatial.orient.euler);
        break;
    }
}

void xCam::refresh_mat()
{
    refresh_mat(mat, spatial);
    if (flags & 0x80) {
        refresh_mat(coll_mat, coll_spatial);
    } else {
        coll_mat = mat;
        coll_spatial = spatial;
    }
}

void xCam::create()
{
    flags = 0;
    coord_type = XCAM_COORD_INVALID;
    orient_type = XCAM_ORIENT_INVALID;
    group = NULL;
    debug_init();
}

// UNUSED: needed to match (speculative code)
bool xCam::blendingTo() const
{
    return blending() && blender->dst == this;
}

void xCamGroup::create()
{
    for (xCamBlend** it = blend_cam, **end = it + ARRAY_LENGTH(blend_cam); it != end; it++)
        *it = create_blend();
}

void xCamGroup::destroy()
{
    primary = NULL;
    for (xCamBlend** it = blend_cam, **end = it + ARRAY_LENGTH(blend_cam); it != end; it++)
        (*it)->destroy();
}

void xCamGroup::reset()
{
    if (primary) {
        stop(*primary);
    }

    primary = NULL;
    flags = 0;
    fov = fov_default;
    set_child_flags(0, 0);

    coll_mat = g_I3;
    coll_atXZ = coll_mat.at;
    mat = g_I3;
    cameraPlayerAudioBias = 0.0f;
}

void xCamGroup::update(xScene& scene, F32 dt)
{
    xVec3 prev_pos = mat.pos;

    do_transient_replacement(primary);

    if (primary) {
        primary->analog = analog;
        primary->do_update(scene, dt);

        mat = primary->mat;
        coll_mat = primary->coll_mat;

        F32 len;
        xVec3NormalizeXZMacro(&coll_atXZ, &coll_mat.at, &len);
        coll_atXZ.y = 0.0f;
        fov = primary->fov;
        if (dt < SECS_PER_VBLANK)
            dt = SECS_PER_VBLANK;
        vel.ScaleDiff(mat.pos, prev_pos, 1.0f / dt);

        cameraPlayerAudioBias = primary->getCameraPlayerAudioBias();
    }
}

void xCamGroup::transition_to(xCam& cam, const xCamTransitionParams* params, bool force_cut, bool force_restart)
{
    if (!force_restart && primary && primary->owner == cam.owner)
        return;

    if (force_cut || !params) {
        set_primary(&cam, force_restart);
        return;
    }

    xCamTransition* transition = params ? params->mTransitionObject : NULL;
    xCam* next = get_blend(transition, cam);
    set_primary(next, false);
}

void xCamGroup::add(xCam& cam, bool force_cut)
{
    transition_to(cam, NULL, force_cut, false);
}

void xCamGroup::remove(xCam& cam, bool)
{
    if (cam.group == this)
        stop(cam);
}

xCamBlend* xCamGroup::grab_blend_cam()
{
    for (xCamBlend** it = blend_cam, **end = it + ARRAY_LENGTH(blend_cam); it != end; it++) {
        if (!(*it)->group)
            return *it;
    }
    return NULL;
}

void xCamGroup::set_primary(xCam* toCamera, bool force_restart)
{
    if (!force_restart && primary && primary == toCamera) {
        return;
    }

    if (primary && !(primary->group_flags & 0x10)) {
        stop(*primary);
        mat = primary->mat;
        coll_mat = primary->coll_mat;

        F32 len;
        xVec3NormalizeXZMacro(&coll_atXZ, &coll_mat.at, &len);
        coll_atXZ.y = 0.0f;
    }

    if (!toCamera) {
        return;
    }

    primary = toCamera;
    primary->group = this;
    primary->mat = mat;
    primary->coll_mat = coll_mat;
    primary->fov = fov_default;
    primary->start();
}

xCam* xCamGroup::get_blend(xCamTransition* transition, xCam& to)
{
    if (primary) {
        F32 blend_time = to.cfg_common.blend_time;

        if (transition) {
            if (transition->IsCut()) goto no_blend;
        }
        else if (blend_time <= 0.0f) goto no_blend;

        if (to.group_flags & 0x10) {
            xCamBlend* parent = to.blender;
            xCam* pTo = &to;
            while (parent) {
                if (parent->dst != pTo)
                    parent->reverse();
                pTo = parent;
                parent = parent->blender;
            }
            return primary;
        }

        xCamBlend* blend = grab_blend_cam();
        if (!blend) return &to;

        blend->attach(*primary, to);

        to.group = this;

        if (transition) {
            blend->bias = xCamBias::GetBiasObject(transition, blend->timeBias);
        } else {
            blend->timeBias.Init(blend_time);
            blend->bias = &blend->timeBias;
        }

        blend->transition = transition;
        return blend;
    }

no_blend:
    if (transition) {
        transition->Completed(false);
    }

    return &to;
}

void xCamGroup::notify_released(xCam& cam)
{
    cam.group_flags &= ~0x10;
    cam.blender = NULL;
}

void xCamGroup::set_child_flags(S32 add, S32 remove)
{
    child_flags_mask = ~remove;
    child_flags = add & child_flags_mask;
}

static void do_transient_replacement(xCam*& curCam)
{
    xCam* next;
    while (curCam && (curCam->flags & 0x4) && (next = curCam->get_next()) != curCam) {
        if (curCam->started()) {
            curCam->stop();
        }
        curCam->group = NULL;
        curCam = next;
    }
}

xCamBlend* xCamGroup::create_blend()
{
    xCamBlend* cam = new (eMemStaticType_0, eMemMgrTag_Camera) xCamBlend();
    if (!cam) return NULL;
    cam->create();
    cam->owner = 'XCBL';
    cam->cfg_common.priority = 0xff;
    return cam;
}

void xCamScreen::create(S32 w, S32 h)
{
    icam = iCameraCreate(w, h, TRUE);
}

void xCamScreen::destroy()
{
    iCameraDestroy(icam);
}

void xCamScreen::set_scene(xScene& scene)
{
    iCameraAssignEnv(icam, scene.env->geom);
}

void xCamScreen::prepare_render()
{
    UpdateGlobalFrustumPlanes(icam);
}

void xCamScreen::begin_render(bool clear)
{
    iCameraBegin(icam, clear);
    iCameraUpdateFog(icam, 0);
}

void xCamScreen::end_render()
{
    iCameraEnd(icam);
}

void xCamScreen::set_world_matrix(const xMat4x3& mat)
{
    iCameraUpdatePos(icam, (xMat4x3*)&mat);
}

void xCamScreen::set_fov(F32 fov)
{
    this->fov = fov;
    iCameraSetFOV(icam, (180.0f/PI) * fov);
}

F32 xCamScreen::world_to_screen(xVec3& screen_loc, const xVec3& world_loc) const
{
    const xMat4x3& view_mat = *(const xMat4x3*)RwCameraGetViewMatrix(icam);
    xVec3 view_loc;
    xMat4x3ToWorld(&view_loc, &view_mat, &world_loc);
    F32 rz = 1.0f / view_loc.z;
    screen_loc.assign(view_loc.x * rz, view_loc.y * rz, 1.0f);
    return rz;
}

// UNUSED: needed to match (speculative code)
struct plane_type
{
    xVec3 norm;
    F32 dist;
};

// UNUSED: needed to match (speculative code)
void xCamScreen::create_frustum_planes(xVec4* frust_planes, const xMat4x3& ltm, const xVec2& view_offset, const xVec2& view_window, F32 near_plane, F32 far_plane)
{
    plane_type* planes = (plane_type*)frust_planes;

    xVec3 temp, temp2;
    temp.Scale(ltm.right, view_offset.x);
    temp2.Scale(ltm.up, view_offset.y);

    xVec3 right, up, cop;
    cop.Sub(temp2, temp);
    F32 scale = 2.0f;

    right.Scale(ltm.right, view_window.x);
    up.Scale(ltm.up, view_window.y);

    xVec3 corners[8];
    S32 i;
    corners[0].Add(ltm.at, right);
    corners[0] += up;

    right *= scale;
    up *= scale;

    corners[1].Sub(corners[0], right);
    corners[2].Sub(corners[1], up);
    corners[3] = corners[0];
    corners[3] -= up;

    for (i = 0; i < 4; i++) {
        xVec3* target = &corners[i];
        temp.Sub(*target, cop);
        *target = cop;
        *target += ltm.pos;
        target->AddScale(temp, near_plane);
        target[4] = cop;
        target[4] += ltm.pos;
        target[4].AddScale(temp, far_plane);
    }

    planes[0].norm = ltm.at;
    planes[0].dist = planes[0].norm.dot(corners[4]);
    planes[1].norm.Negate(planes[0].norm);
    planes[1].dist = planes[1].norm.dot(corners[0]);

    temp.Sub(corners[1], corners[5]);
    temp2.Sub(corners[6], corners[5]);
    planes[2].norm.cross(temp, temp2);
    planes[2].norm.normalize();
    planes[2].dist = planes[2].norm.dot(corners[1]);

    temp2.Sub(corners[4], corners[5]);
    planes[3].norm.cross(temp2, temp);
    planes[3].norm.normalize();
    planes[3].dist = planes[3].norm.dot(corners[1]);

    temp.Sub(corners[3], corners[7]);
    temp2.Sub(corners[4], corners[7]);
    planes[4].norm.cross(temp, temp2);
    planes[4].norm.normalize();
    planes[4].dist = planes[4].norm.dot(corners[3]);

    temp2.Sub(corners[6], corners[7]);
    planes[5].norm.cross(temp2, temp);
    planes[5].norm.normalize();
    planes[5].dist = planes[5].norm.dot(corners[3]);
}

void xCamTimeBias::Init(F32 blendTime)
{
    this->blendTime = blendTime;
}

void xCamTimeBias::Start()
{
    time = 0.0f;
    bias = 0.0f;
}

void xCamTimeBias::Update(F32 dt)
{
    time += dt;
    CalcBias();
}

void xCamTimeBias::Reverse()
{
    time = blendTime - time;
    CalcBias();
}

F32 xCamTimeBias::GetBias()
{
    return bias;
}

bool xCamTimeBias::IsActive()
{
    return time < blendTime;
}

bool xCamTimeBias::DidComplete()
{
    return time >= blendTime;
}

void xCamTimeBias::CalcBias()
{
    F32 t = time / blendTime; if (t > 1.0f) t = 1.0f;
    bias = xSCurve(t);
}

xCamBias* xCamBias::GetBiasObject(xCamTransition* transition, xCamTimeBias& timeBias)
{
    if (transition->BlendBias()) {
        return transition->BlendBias();
    }

    timeBias.Init(transition->BlendTime());
    return &timeBias;
}

void xCamBlend::create()
{
    xCam::create();
    flags |= 0x4;
    coord_type = XCAM_COORD_INVALID;
    orient_type = XCAM_ORIENT_INVALID;
}

void xCamBlend::destroy()
{
}

void xCamBlend::start()
{
    static const xCamCoordType coord_table[3][3] = {
        { XCAM_COORD_CART, XCAM_COORD_CART, XCAM_COORD_CART },
        { XCAM_COORD_CART, XCAM_COORD_CYLINDER, XCAM_COORD_SPHERE },
        { XCAM_COORD_CART, XCAM_COORD_SPHERE, XCAM_COORD_SPHERE }
    };

    static const xCamOrientType orient_table[2][2] = {
        { XCAM_ORIENT_QUAT, XCAM_ORIENT_QUAT },
        { XCAM_ORIENT_QUAT, XCAM_ORIENT_EULER }
    };

    if (!src->started()) {
        src->mat = mat;
        src->coll_mat = coll_mat;
        src->fov = fov;
        src->start();
    }

    if (!dst->started()) {
        dst->mat = mat;
        dst->coll_mat = coll_mat;
        dst->fov = fov;
        dst->start();
    }

    cfg_common.priority = MAX(src->cfg_common.priority, dst->cfg_common.priority);

    coord_type = coord_table[src->coord_type][dst->coord_type];
    orient_type = orient_table[src->orient_type][dst->orient_type];

    S32 combined_sprung_flags = (src->flags | dst->flags) & 0x80;
    if (combined_sprung_flags)
        flags |= 0x80;
    else
        flags &= ~0x80;

    bias->Start();
    xCam::start();
}

void xCamBlend::stop()
{
    group->notify_released(*src);
    group->notify_released(*dst);

    if (bias->DidComplete()) {
        if (bias->GetBias() < 0.5f) {
            group->stop(*dst);
        } else {
            group->stop(*src);
        }
    } else {
        group->stop(*src);
        group->stop(*dst);
    }

    if (transition) {
        transition->Completed(bias->DidComplete());
        transition = NULL;
    }

    xCam::stop();
}

void xCamBlend::pre_update(xScene& scene)
{
    src->pre_update(scene);
    dst->pre_update(scene);
}

void xCamBlend::post_update(xScene& scene)
{
    src->post_update(scene);
    dst->post_update(scene);
}

void xCamBlend::update(xScene& scene, F32 dt)
{
    bias->Update(dt);
    F32 s = bias->GetBias();

    do_transient_replacement(src);
    do_transient_replacement(dst);

    src->analog = analog;
    src->update(scene, dt);

    dst->analog = analog;
    dst->update(scene, dt);

    blend_coord(s);
    blend_orient(s);
    blend(fov, src->fov, dst->fov, s);

    refresh_mat();
}

xCam* xCamBlend::get_next()
{
    return bias->IsActive() ? this : (bias->GetBias() < 0.5f ? src : dst);
}

F32 xCamBlend::getCameraPlayerAudioBias() const
{
    F32 srcBias = src->getCameraPlayerAudioBias();
    F32 dstBias = dst->getCameraPlayerAudioBias();
    return srcBias + (dstBias - srcBias) * bias->GetBias();
}

xCam* xCamBlend::find_camera(U32 ownerID)
{
    xCam* found = src->find_camera(ownerID);
    return found ? found : dst->find_camera(ownerID);
}

void xCamBlend::attach(xCam& src, xCam& dst)
{
    src.blender = dst.blender = this;
    src.group_flags |= 0x10;
    dst.group_flags |= 0x10;
    group_flags = 0x1;
    this->src = &src;
    this->dst = &dst;
}

void xCamBlend::reverse()
{
    xCam* temp = src;
    src = dst;
    dst = temp;
    bias->Reverse();
}

void xCamBlend::blend_coord(F32 s)
{
    switch (coord_type) {
    case XCAM_COORD_CART: {
        xVec3 c1, c2;
        blend(spatial.coord.cart, convert(*src, c1), convert(*dst, c2), s);
        blend(coll_spatial.coord.cart, coll_convert(*src, c1), coll_convert(*dst, c2), s);
        break;
    }
    case XCAM_COORD_CYLINDER: {
        xCamCoordCylinder c1, c2;
        blend(spatial.coord.cylinder, convert(*src, c1), convert(*dst, c2), s);
        blend(coll_spatial.coord.cylinder, coll_convert(*src, c1), coll_convert(*dst, c2), s);
        break;
    }
    case XCAM_COORD_SPHERE: {
        xCamCoordSphere c1, c2;
        blend(spatial.coord.sphere, convert(*src, c1), convert(*dst, c2), s);
        blend(coll_spatial.coord.sphere, coll_convert(*src, c1), coll_convert(*dst, c2), s);
        break;
    }
    }
}

void xCamBlend::blend_orient(F32 s)
{
    switch (orient_type) {
    case XCAM_ORIENT_QUAT: {
        xQuat c1, c2;
        blend(spatial.orient.quat, convert(*src, c1), convert(*dst, c2), s);
        blend(coll_spatial.orient.quat, coll_convert(*src, c1), coll_convert(*dst, c2), s);
        break;
    }
    case XCAM_ORIENT_EULER: {
        xCamOrientEuler c1, c2;
        blend(spatial.orient.euler, convert(*src, c1), convert(*dst, c2), s);
        blend(coll_spatial.orient.euler, coll_convert(*src, c1), coll_convert(*dst, c2), s);
        break;
    }
    }
}

// UNUSED: needed to match (code from Rat GC)
void xCamDebug::create()
{
    xCam::create();
    coord_type = XCAM_COORD_CART;
    orient_type = XCAM_ORIENT_EULER;
}

// UNUSED: needed to match (code from Rat GC)
void xCamDebug::start()
{
    xCam::start();
    coord_to_local(spatial.coord.cart, mat.pos);
    orient_to_local(spatial.orient.euler, mat);
    dpad = 0.0f;
    slow_input = 0.0f;
    spatial.orient.euler.roll = 0.0f;
}

// UNUSED: needed to match (code from Rat GC)
void xCamDebug::update(xScene&, F32 dt)
{
    F32 turn_mult = 1.0f + (cfg.slow_turn - 1.0f) * slow_input;
    F32 move_mult = 1.0f + (cfg.slow_move - 1.0f) * slow_input;

    F32 dyaw = dt * (turn_mult * (cfg.stick_vel.x * zPad::GetDebugPad()->GetAnalog2()->GetX()));
    spatial.orient.euler.yaw += dyaw;
    spatial.orient.euler.yaw = xrmod(spatial.orient.euler.yaw);

    F32 dpitch = dt * (turn_mult * (cfg.stick_vel.y * zPad::GetDebugPad()->GetAnalog2()->GetY()));
    spatial.orient.euler.pitch += dpitch;
    spatial.orient.euler.pitch = range_limit(spatial.orient.euler.pitch, -89.0f * (PI/180.0f), 89.0f * (PI/180.0f));

    orient_to_world(mat, spatial.orient.euler);

    xVec3 dright = mat.right;
    dright *= dt * (move_mult * (cfg.move_vel.x * zPad::GetDebugPad()->GetAnalog1()->GetX()));
    spatial.coord.cart += dright;

    F32 dy = dt * (move_mult * (-dpad.y * cfg.move_vel.y));
    spatial.coord.cart.y += dy;

    xVec3 dat = mat.at;
    dat *= dt * (move_mult * (cfg.move_vel.z * zPad::GetDebugPad()->GetAnalog1()->GetY()));
    spatial.coord.cart += dat;

    coord_to_world(mat.pos, spatial.coord.cart);
}
