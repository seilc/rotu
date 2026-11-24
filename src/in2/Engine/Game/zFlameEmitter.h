#ifndef ZFLAMEEMITTER_H
#define ZFLAMEEMITTER_H

#include "xBase.h"
#include "zFire.h"

struct xDynAsset;

struct zFlameEmitter : public xBase
{
    struct asset_type;

    const asset_type* asset;
    xMat4x3 mat;

    void load(const asset_type& a);
    void reset();
    void hide();
    void show();
    void spawn();
    void kill(F32);
    void update(F32 dt);
    void render();
    static void load(xBase& data, xDynAsset& asset, size_t asset_size);
    void get_bounding_box(xBox& box) const;
    static void scene_enter();
    static void scene_exit();
    static void setup_all();
    static void update_all(F32 dt);
    static void render_all();
    static void fire_reset();
    void refresh_mat();
    void refresh_properties();
    void setup();
    void destroy();
    void create_flames(S32 flags);
    void destroy_flames();
    void dump_flames();
    void start_flames_sound();
    void end_flames_sound();
    static void cb_dispatch(xBase*, xBase* to, U32 event, const F32* argf, xBase*, U32);
    void debug_init();
    void debug_render();

private:
    S32 flags;
    zFire::handle* flames;
    zFire::property_set props;
    S32 flames_size;
    S32 flames_allocated;
    iSndGroupHandle sound_group;
    xSndHandle sound_playing;
    F32 oscil;
    xVec3 driverOffset;
};

#endif
