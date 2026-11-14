#include "xCommon.h"
#include "zCineFXTableSceneB312.h"

#include "zCineFX.h"
#include "zCineFXShared.h"
#include "zFrozoneIce.h"
#include "xutil.h"

namespace CineFXSceneB312 {

namespace {

const zFrozoneIceRecordEntry record_b3_e_03[] = {
    {  0.041500f, {  -2.94145f,  48.90862f, -30.21483f } },
    {  0.129000f, {  -3.31050f,  49.14700f, -29.41757f } },
    {  0.172000f, {  -3.81754f,  49.29223f, -28.90441f } },
    {  0.214000f, {  -4.59017f,  49.28133f, -28.43827f } },
    {  0.234000f, {  -5.05524f,  49.24912f, -28.19363f } },
    {  0.255500f, {  -5.61883f,  49.21654f, -27.91057f } },
    {  0.276500f, {  -6.10127f,  49.15916f, -27.58055f } },
    {  0.297500f, {  -6.52445f,  49.05885f, -27.24002f } },
    {  0.319000f, {  -6.86311f,  48.80742f, -26.88837f } },
    {  0.340500f, {  -7.17918f,  48.48338f, -26.53796f } },
    {  0.362000f, {  -7.45672f,  48.08822f, -26.17693f } },
    {  0.383500f, {  -7.71448f,  47.65130f, -25.84381f } },
    {  0.404000f, {  -7.89468f,  47.22710f, -25.50728f } },
    {  0.426000f, {  -8.06246f,  46.77509f, -25.17729f } },
    {  0.468500f, {  -8.22932f,  46.03733f, -24.57038f } },
    {  0.511500f, {  -8.05447f,  45.42105f, -24.02579f } },
    {  0.554000f, {  -7.51994f,  44.82420f, -23.50743f } },
    {  0.575500f, {  -7.14806f,  44.53427f, -23.23723f } },
    {  0.596000f, {  -6.74759f,  44.26689f, -22.96585f } },
    {  0.617500f, {  -5.77284f,  43.70870f, -22.30922f } },
    {  0.659000f, {  -5.32603f,  43.47803f, -22.00626f } },
    {  0.680000f, {  -4.39922f,  43.05988f, -21.39253f } },
    {  0.723000f, {  -3.97288f,  42.88784f, -21.10395f } },
    {  0.744500f, {  -3.21582f,  42.62564f, -20.54794f } },
    {  0.765000f, {  -3.07460f,  42.58743f, -20.42681f } },
};

const cinfx_frobridge_context frotext_b3_e_03 = { 0, record_b3_e_03, ARRAY_LENGTH(record_b3_e_03) };

const zFrozoneIceRecordEntry record_b3_e_05[] = {
    {  0.035000f, {   4.04995f,  43.91131f, -23.49807f } },
    {  0.044500f, {   1.04995f,  41.91131f, -20.49807f } },
    {  0.078500f, {   0.89575f,  41.43651f, -20.19043f } },
    {  0.112500f, {   0.45795f,  40.07834f, -19.30906f } },
    {  0.147000f, {   0.02908f,  38.70341f, -18.43100f } },
    {  0.180000f, {  -0.36586f,  37.38853f, -17.60338f } },
    {  0.213000f, {  -0.74509f,  36.07065f, -16.77926f } },
    {  0.246500f, {  -1.11690f,  34.72486f, -15.94909f } },
    {  0.279500f, {  -1.47134f,  33.38876f, -15.13764f } },
    {  0.312500f, {  -1.81750f,  32.03953f, -14.33369f } },
    {  0.345000f, {  -2.15314f,  30.69630f, -13.54991f } },
    {  0.377500f, {  -2.48749f,  29.33773f, -12.77491f } },
    {  0.409500f, {  -2.82046f,  27.98520f, -12.02063f } },
    {  0.441000f, {  -3.15571f,  26.64092f, -11.28669f } },
    {  0.472000f, {  -3.49784f,  25.30813f, -10.57191f } },
    {  0.503500f, {  -3.85824f,  23.94618f,  -9.85260f } },
};

const cinfx_frobridge_context frotext_b3_e_05 = { 0, record_b3_e_05, ARRAY_LENGTH(record_b3_e_05) };

const zFrozoneIceRecordEntry record_b3_e_06[] = {
    {  0.033500f, {  -5.07296f,   1.55712f,   8.66206f } },
    {  0.123500f, {  -4.79184f,   1.12153f,   9.45668f } },
    {  0.167000f, {  -4.56859f,   0.81179f,  10.01954f } },
    {  0.232500f, {  -4.23897f,   0.43221f,  10.79722f } },
    {  0.321000f, {  -3.89023f,   0.15710f,  11.60090f } },
    {  0.452000f, {  -3.85139f,   0.07954f,  12.52152f } },
    {  0.581500f, {  -3.99445f,   0.04556f,  13.40987f } },
    {  0.798500f, {  -4.11260f,   0.03219f,  14.38381f } },
    {  1.164100f, {  -4.13704f,   0.03659f,  14.66172f } },
};

const cinfx_frobridge_context frotext_b3_e_06 = { 0, record_b3_e_06, ARRAY_LENGTH(record_b3_e_06) };

} // namespace

static NCINEntry g_cinfx_B312_03_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(200), "B3_E_03 Ice Glide" },
      { cinfx_frobridge_update, cinfx_frobridge_animate, cinfx_frobridge_render, cinfx_frobridge_add_tweaks },
      { 1, 51, 55 },
      { 0 },
      (void*)&frotext_b3_e_03 },
    { 0 }
};

static NCINEntry g_cinfx_B312_05_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(200), "B3_E_05 Ice Glide" },
      { cinfx_frobridge_update, cinfx_frobridge_animate, cinfx_frobridge_render, cinfx_frobridge_add_tweaks },
      { 2, 51, 55 },
      { 0 },
      (void*)&frotext_b3_e_05 },
    { 0 }
};

static NCINEntry g_cinfx_B312_06_manager[] = {
    { { 0x1000, CINFX_FRAMES(0), CINFX_FRAMES(200), "B3_E_06 Ice Glide" },
      { cinfx_frobridge_update, cinfx_frobridge_animate, cinfx_frobridge_render, cinfx_frobridge_add_tweaks },
      { 1, 51, 55 },
      { 0 },
      (void*)&frotext_b3_e_06 },
    { 0 }
};

} // namespace CineFXSceneB312

void zCineFX_Register_SceneB312()
{
    static NCINMap cutmap03[] = {
        { "B3E_030", CineFXSceneB312::g_cinfx_B312_03_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap05[] = {
        { "B3E_050", CineFXSceneB312::g_cinfx_B312_05_manager, 0 },
        { NULL, NULL, 0 }
    };
    static NCINMap cutmap06[] = {
        { "B3E_060", CineFXSceneB312::g_cinfx_B312_06_manager, 0 },
        { NULL, NULL, 0 }
    };

    zCineFXRegisterTable(cutmap03);
    zCineFXRegisterTable(cutmap05);
    zCineFXRegisterTable(cutmap06);
}
