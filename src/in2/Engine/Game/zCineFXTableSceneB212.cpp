#include "xCommon.h"
#include "zCineFXTableSceneB212.h"

#include "zCineFX.h"
#include "zCineFXShared.h"
#include "zFrozoneIce.h"
#include "zFXBossBrain.h"
#include "xutil.h"

#define FX_HEAD_CRUST_ID 0x0A5FABE1 // xStrHash("fx_head_crust")

namespace CineFXSceneB212 {

namespace {

const zFrozoneIceRecordEntry record_b2_e_01[] = {
    {  0.000000f, {  -6.72430f,   2.15331f,   1.78719f } },
    {  0.079371f, {  -5.81500f,   2.07927f,   1.50336f } },
    {  0.189832f, {  -5.10793f,   2.08927f,   0.80194f } },
    {  0.313648f, {  -4.98354f,   2.04050f,  -0.18556f } },
    {  0.433967f, {  -4.88089f,   1.99339f,  -1.16089f } },
    {  0.519532f, {  -5.17575f,   1.91236f,  -2.05981f } },
    {  0.592582f, {  -5.63385f,   1.84278f,  -2.93549f } },
    {  0.661419f, {  -6.07745f,   1.80535f,  -3.76492f } },
    {  0.730350f, {  -6.54847f,   1.72520f,  -4.62544f } },
    {  0.794400f, {  -7.00569f,   1.69057f,  -5.43884f } },
    {  0.858313f, {  -7.46897f,   1.70024f,  -6.27135f } },
    {  0.927094f, {  -7.47514f,   1.57991f,  -7.25246f } },
    {  1.034043f, {  -7.33175f,   1.45821f,  -8.22842f } },
    {  1.108814f, {  -7.20014f,   1.34638f,  -9.15553f } },
    {  1.183740f, {  -7.01239f,   1.22852f, -10.09309f } },
    {  1.258755f, {  -6.79831f,   1.10751f, -11.03034f } },
    {  1.333740f, {  -6.58433f,   0.98665f, -11.96753f } },
    {  1.408797f, {  -6.37019f,   0.86578f, -12.90595f } },
    {  1.479342f, {  -6.15596f,   0.74779f, -13.83453f } },
    {  1.540518f, {  -5.93472f,   0.64854f, -14.78315f } },
    {  1.630165f, {  -5.79658f,   0.59191f, -15.76330f } },
    {  1.719733f, {  -5.75331f,   0.39587f, -16.73189f } },
};

const cinfx_frobridge_context frotext_b2_e_01 = { 0, record_b2_e_01, ARRAY_LENGTH(record_b2_e_01) };

const cinfx_bossbrain_context braintext_b2_e_01 = { FX_HEAD_CRUST_ID, 10, 0.1f, 0.3f };

} // namespace

static NCINEntry g_cinfx_B212_01_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(75), "B2_E_01 Ice Glide" },
      { cinfx_frobridge_update, cinfx_frobridge_animate, cinfx_frobridge_render, cinfx_frobridge_add_tweaks },
      { 0, 51, 55 },
      { 0 },
      (void*)&frotext_b2_e_01 },
    { { 0x1001, CINFX_FRAMES(0), CINFX_FRAMES(200), "B2_E_01 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 2, 2 },
      { { { 0.0f, 0.65f, 0.0f } } },
      (void*)&braintext_b2_e_01 },
    { 0 }
};

static NCINEntry g_cinfx_B212_05_manager[] = {
    { { 0x1001, CINFX_FRAMES(0), CINFX_FRAMES(200), "B2_E_05 Boss Brain" },
      { zFXBossBrain::cinfxUpdate, zFXBossBrain::cinfxAnimate, zFXBossBrain::cinfxRender,
        zFXBossBrain::cinfxAddTweaks },
      { 1, 2 },
      { { { 0.0f, 0.55f, 0.0f } } },
      (void*)&braintext_b2_e_01 },
    { 0 }
};

} // namespace CineFXSceneB212

void zCineFX_Register_SceneB212()
{
    static NCINMap cutmap01[] = {
        { "B2E_010", CineFXSceneB212::g_cinfx_B212_01_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap05[] = {
        { "B2E_050", CineFXSceneB212::g_cinfx_B212_05_manager, 0 },
        { NULL, NULL, 0 }
    };

    zCineFXRegisterTable(cutmap01);
    zCineFXRegisterTable(cutmap05);
}
