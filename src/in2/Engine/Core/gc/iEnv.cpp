#include "xCommon.h"
#include "iEnv.h"

#include "xClumpColl.h"
#include "xModel.h"
#include "xRenderState.h"
#include "xString.h"
#include "zJSPExtraData.h"
#include "zMain.h"
#include "iCamera.h"
#include "iMath3.h"
#include "iModel.h"

#include <rpmatfx.h>
#include <string.h>

static S32 sBeginDrawFX;
static RpWorld* sPipeWorld;
static RwCamera* sPipeCamera;

static RpGameCubeTevOp sTevOpAddLightmap = {
    rpGX_CC_ZERO, rpGX_CC_TEXC, rpGX_CC_ONE, rpGX_CC_RASC,
    rpGX_CA_ZERO, rpGX_CA_ZERO, rpGX_CA_ZERO, rpGX_CA_RASA,
    rpGX_TEV_ADD, rpGX_TB_ZERO, rpGX_CS_SCALE_1, TRUE, rpGX_TEVPREV,
    rpGX_TEV_ADD, rpGX_TB_ZERO, rpGX_CS_SCALE_1, TRUE, rpGX_TEVPREV,
    rpGX_TEV_KCSEL_1, rpGX_TEV_KASEL_1
};

static RpGameCubeTevOp sTevOpModulate = {
    rpGX_CC_ZERO, rpGX_CC_CPREV, rpGX_CC_TEXC, rpGX_CC_ZERO,
    rpGX_CA_ZERO, rpGX_CA_TEXA, rpGX_CA_APREV, rpGX_CA_ZERO,
    rpGX_TEV_ADD, rpGX_TB_ZERO, rpGX_CS_SCALE_1, TRUE, rpGX_TEVPREV,
    rpGX_TEV_ADD, rpGX_TB_ZERO, rpGX_CS_SCALE_1, TRUE, rpGX_TEVPREV,
    rpGX_TEV_KCSEL_1, rpGX_TEV_KASEL_1
};

static RpGameCubeTexGen sTexGenIdentity = {
    0, rpGX_TG_MTX2x4, rpGX_TG_TEX0, rpGX_IDENTITY, rpGX_PTIDENTITY, 0, 0
};

static RpMTEffect* sEffectLightMap;

static void iEnvMatOrderSetup(iEnv* env);
static void iEnvMatOrderRenderGC(iEnv* env, bool alpha);

static RpAtomic* SetPipelineCB(RpAtomic* atomic, void*)
{
    RpMaterialList* matList = &atomic->geometry->matList;
    for (S32 i = 0; i < matList->numMaterials; i++) {
        U32 matEffect = RpMatFXMaterialGetEffects(matList->materials[i]);
        if (matEffect == rpMATFXEFFECTDUAL) {
            RpGeometry* geom = RpAtomicGetGeometry(atomic);
            RwInt32 vert_size = RpGeometryGetNumVertices(geom);
            RwRGBA* vert_colors = RpGeometryGetPreLightColors(geom);

            if (vert_colors) {
                for (S32 j = 0; j < vert_size; j++) {
                    vert_colors[j].red = vert_colors[j].green = vert_colors[j].blue = 0;
                }
            }

            RwTexture* baseTexture = RpMaterialGetTexture(matList->materials[i]);
            RwTexture* lightmapTexture = RpMatFXMaterialGetDualTexture(matList->materials[i]);

            RpMaterial* material = RpMaterialCreateMultiTexture(matList->materials[i], rwID_GAMECUBE, 2);
            RpMultiTexture* multiTexture = RpMaterialGetMultiTexture(material, rwID_GAMECUBE);
            RpMultiTextureSetEffect(multiTexture, sEffectLightMap);

            RpMultiTextureSetTexture(multiTexture, 0, lightmapTexture);
            RpMultiTextureSetTexture(multiTexture, 1, baseTexture);
            RpMultiTextureSetCoords(multiTexture, 0, 1);
            RpMultiTextureSetCoords(multiTexture, 1, 0);
        }
    }

    return atomic;
}

void iEnvStartup()
{
    sEffectLightMap = RpGameCubeMTEffectCreate(2, 2, 0, 0, 0);
    RpGameCubeMTConfig* config = RpGameCubeMTEffectGetConfig(sEffectLightMap);

    config->tevStages[0].op = sTevOpAddLightmap;
    config->tevStages[0].channelID = rpGX_COLOR0A0;
    config->tevStages[0].texCoordID = rpGX_TEXCOORD0;
    config->tevStages[0].texMapID = rpGX_TEXMAP0;
    config->tevStages[0].flags = 0;
    config->texGens[0] = sTexGenIdentity;
    config->texGens[0].srcParam = rpGX_TG_TEX0;

    config->tevStages[1].op = sTevOpModulate;
    config->tevStages[1].channelID = rpGX_COLOR0A0;
    config->tevStages[1].texCoordID = rpGX_TEXCOORD1;
    config->tevStages[1].texMapID = rpGX_TEXMAP1;
    config->tevStages[1].flags = 0;
    config->texGens[1] = sTexGenIdentity;
    config->texGens[1].srcParam = rpGX_TG_TEX1;
}

void iEnvLoadBegin(iEnv* env, S32 dataType, S32 count)
{
    if (dataType == 0) {
        env->jsp_aid = (U32*)xMEMALLOC(count * sizeof(U32), 0);
        env->jsp_list = (xJSPHeader**)xMEMALLOC(count * sizeof(xJSPHeader*), 0);
        env->jsp_bound = (xBox*)xMEMALLOC(count * sizeof(xBox), 0);
        env->jsp_count = count;
        env->jsp_visibilityCount = (S32*)xMEMALLOC(count * sizeof(S32), 0);

        RwBBox tmpbbox = { 1000.0f, 1000.0f, 1000.0f, -1000.0f, -1000.0f, -1000.0f };
        env->world = RpWorldCreate(&tmpbbox);

        sPipeCamera = iCameraCreate(FB_XRES, FB_YRES, 0);
        sPipeWorld = env->world;
        RpWorldAddCamera(sPipeWorld, sPipeCamera);
    }
}

bool iEnvLoadJSP(iEnv* env, U32 aid, const void* data, U32, S32 dataType, S32 index)
{
    xJSPHeader* jsp = (xJSPHeader*)data;
    if (jsp->idtag[0] == 'J' && jsp->idtag[1] == 'S' && jsp->idtag[2] == 'P' && jsp->idtag[3] == '\0' && dataType == 0) {
        env->jsp_aid[index] = aid;
        env->jsp_list[index] = jsp;
        env->jsp_visibilityCount[index] = 0;
        RpClumpForAllAtomics(env->jsp_list[index]->clump, SetPipelineCB, NULL);
        xClumpColl_InstancePointers(env->jsp_list[index]->colltree, env->jsp_list[index]->clump);
        iBuildJSPBound(env->jsp_list[index]->colltree, env->jsp_bound[index]);
        return true;
    }

    return false;
}

void iEnvJSPVisibilityInc(iEnv* env, xJSPHeader* jsp)
{
    for (S32 i = 0; i < env->jsp_count; i++) {
        if (env->jsp_list[i] == jsp) {
            if (env->jsp_visibilityCount[i] == 0) {
                zJSPExtraData::ShowAssociatedGroup(env->jsp_aid[i], true);
            }
            env->jsp_visibilityCount[i]++;
            return;
        }
    }
}

void iEnvJSPVisibilityDec(iEnv* env, xJSPHeader* jsp)
{
    for (S32 i = 0; i < env->jsp_count; i++) {
        if (env->jsp_list[i] == jsp) {
            if (env->jsp_visibilityCount[i] == 1) {
                zJSPExtraData::ShowAssociatedGroup(env->jsp_aid[i], false);
            }
            env->jsp_visibilityCount[i]--;
            return;
        }
    }
}

void iEnvJSPOn(iEnv* env, xJSPHeader* jsp)
{
    for (S32 i = 0; i < env->jsp_count; i++) {
        if (env->jsp_list[i] == jsp) {
            if (env->jsp_visibilityCount[i] == 0) {
                zJSPExtraData::ShowAssociatedGroup(env->jsp_aid[i], true);
            }
            env->jsp_visibilityCount[i] = 1;
            return;
        }
    }
}

void iEnvJSPOff(iEnv* env, xJSPHeader* jsp)
{
    for (S32 i = 0; i < env->jsp_count; i++) {
        if (env->jsp_list[i] == jsp) {
            if (env->jsp_visibilityCount[i] == 1) {
                zJSPExtraData::ShowAssociatedGroup(env->jsp_aid[i], false);
            }
            env->jsp_visibilityCount[i] = 0;
            return;
        }
    }
}

void iBuildJSPBound(const xClumpCollBSPTree* colltree, xBox& box)
{
    for (S32 j = 0; j < colltree->numTriangles; j++) {
        for (S32 k = 0; k < 3; k++) {
            iBoxBoundVec(&box, &box, (xVec3*)&colltree->triangles[j].v.p[k]);
        }
    }
}

void iEnvLoadEnd(iEnv* env, S32 dataType)
{
    if (dataType == 0) {
        iEnvMatOrderSetup(env);

        RpWorldRemoveCamera(sPipeWorld, sPipeCamera);
        iCameraDestroy(sPipeCamera);
        sPipeWorld = NULL;
        sPipeCamera = NULL;

        env->memlvl = xMemGetBase();
    }
}

void iEnvFree(iEnv* env)
{
    _rwFrameSyncDirty();

    RpWorldDestroy(env->world);
    env->world = NULL;

    if (env->fx) {
        RpWorldDestroy(env->fx);
        env->fx = NULL;
    }

    if (env->collision) {
        RpWorldDestroy(env->collision);
        env->collision = NULL;
    }
}

void iEnvDefaultLighting(iEnv*)
{
}

void iEnvLightingBasics(iEnv*, xEnvAsset*)
{
}

static iEnv* lastEnv;

void iEnvSetup(iEnv* env)
{
    lastEnv = env;
}

void iEnvRender(iEnv* env, bool alpha)
{
    xRenderStateBlendModesSet(rwBLENDSRCALPHA, rwBLENDINVSRCALPHA);

    if (env->jsp_count > 0) {
        iEnvMatOrderRenderGC(env, alpha);
    } else {
        RpWorldRender(env->world);
    }

    lastEnv = env;
}

void iEnvEndRenderFX(iEnv* env)
{
    env = lastEnv;
    if (env && env->fx && globalCamera && sBeginDrawFX) {
        RpWorldRemoveCamera(env->fx, globalCamera);
        RpWorldAddCamera(env->world, globalCamera);
        sBeginDrawFX = FALSE;
    }
}

static iEnv* sCmpEnv;
static S32 sColorCompare;

static S32 CmpMatOrder(const void* a, const void* b)
{
    const iEnvMatOrder* ael = (const iEnvMatOrder*)a;
    const iEnvMatOrder* bel = (const iEnvMatOrder*)b;

    xJSPNodeInfo* anod = ael->nodeInfo;
    xJSPNodeInfo* bnod = bel->nodeInfo;
    if (anod->sortOrder > bnod->sortOrder) {
        return 1;
    } else if (anod->sortOrder < bnod->sortOrder) {
        return -1;
    }

    S32 decalcmp = (anod->nodeFlags & 0x2) - (bnod->nodeFlags & 0x2);

    if (decalcmp) {
        if (decalcmp > 0) {
            return 1;
        } else {
            return -1;
        }
    }

    RpMaterial* amat = ael->atomic->geometry->matList.materials[0];
    RpMaterial* bmat = bel->atomic->geometry->matList.materials[0];

    RpMatFXMaterialFlags matfx1 = RpMatFXMaterialGetEffects(amat);
    RpMatFXMaterialFlags matfx2 = RpMatFXMaterialGetEffects(bmat);
    if (matfx1 > matfx2) {
        return 1;
    } else if (matfx1 < matfx2) {
        return -1;
    }

    S32 sidecmp = (anod->nodeFlags & 0x4) - (bnod->nodeFlags & 0x4);

    if (sidecmp) {
        if (sidecmp > 0) {
            return 1;
        } else {
            return -1;
        }
    }

    char *atex, *btex;
    if (amat->texture)
        atex = amat->texture->name;
    else
        atex = "";
    if (bmat->texture)
        btex = bmat->texture->name;
    else
        btex = "";
    S32 namecmp = xStricmp(atex, btex);
    if (namecmp) {
        return namecmp;
    }

    if (matfx1 == rpMATFXEFFECTDUAL || matfx1 == rpMATFXEFFECTDUALUVTRANSFORM) {
        RwTexture *dual1, *dual2;
        char *tname1, *tname2;

        dual1 = RpMatFXMaterialGetDualTexture(amat);
        dual2 = RpMatFXMaterialGetDualTexture(bmat);
        if (dual1)
            tname1 = dual1->name;
        else
            tname1 = "";
        if (dual2)
            tname2 = dual2->name;
        else
            tname2 = "";
        namecmp = stricmp(tname1, tname2);
        if (namecmp) {
            return namecmp;
        }
    }

    if (amat->surfaceProps.ambient > bmat->surfaceProps.ambient)
        return 1;
    else if (amat->surfaceProps.ambient < bmat->surfaceProps.ambient)
        return -1;
    if (amat->surfaceProps.specular > bmat->surfaceProps.specular)
        return 1;
    else if (amat->surfaceProps.specular < bmat->surfaceProps.specular)
        return -1;
    if (amat->surfaceProps.diffuse > bmat->surfaceProps.diffuse) {
        return 1;
    } else if (amat->surfaceProps.diffuse < bmat->surfaceProps.diffuse) {
        return -1;
    }

    if (sColorCompare) {
        U32 acol = *(U32*)&amat->color;
        U32 bcol = *(U32*)&bmat->color;
        if (acol > bcol) {
            return 1;
        } else if (acol < bcol) {
            return -1;
        }
    }

    return 0;
}

static void iEnvMatOrderSetup(iEnv* env)
{
    S32 i, currJSPNode = 0, currMatGroup = 0;

    env->jspMatOrderCount = 0;
    for (i = 0; i < env->jsp_count; i++) {
        env->jspMatOrderCount += env->jsp_list[i]->jspNodeCount;
    }
    env->jspMatOrderList = (iEnvMatOrder*)xMEMALLOC(env->jspMatOrderCount * sizeof(iEnvMatOrder), 0);

    for (i = 0; i < env->jsp_count; i++) {
        S32 nodeIndex = 0;
        RwLLLink *cur, *end;
        RpClump* clump = env->jsp_list[i]->clump;
        cur = rwLinkListGetFirstLLLink(&clump->atomicList);
        end = rwLinkListGetTerminator(&clump->atomicList);

        while (cur != end) {
            env->jspMatOrderList[currJSPNode].jspIndex = i;
            env->jspMatOrderList[currJSPNode].nodeIndex = nodeIndex;
            env->jspMatOrderList[currJSPNode].matGroup = 0;
            RpAtomic* apAtom = rwLLLinkGetData(cur, RpAtomic, inClumpLink); env->jspMatOrderList[currJSPNode].atomic = apAtom;
            env->jspMatOrderList[currJSPNode].nodeInfo = &env->jsp_list[i]->jspNodeList[nodeIndex];
            currJSPNode++;
            nodeIndex++;
            cur = rwLLLinkGetNext(cur);
        }
    }

    sCmpEnv = env;
    sColorCompare = TRUE;
    qsort(env->jspMatOrderList, env->jspMatOrderCount, sizeof(iEnvMatOrder), CmpMatOrder);

    for (i = 0; i < env->jspMatOrderCount; i++) {
        if (i == 0 || CmpMatOrder(&env->jspMatOrderList[i - 1], &env->jspMatOrderList[i]) != 0) {
            currMatGroup++;
        }
        env->jspMatOrderList[i].matGroup = currMatGroup;
    }

    env->numOpaque = env->jspMatOrderCount;
    env->numTransparent = 0;
    for (S32 i = 0; i < env->jspMatOrderCount; i++) {
        if (env->jspMatOrderList[i].nodeInfo->sortOrder >= 2) {
            env->numOpaque = i;
            env->numTransparent = env->jspMatOrderCount - i;
            break;
        }
    }
}

static void iEnvMatOrderRenderGC(iEnv* env, bool alpha)
{
    S32 i;
    S32 backcullon = TRUE;
    S32 zbufferon = TRUE;
    S32 matOrderCount = env->jspMatOrderCount;
    iEnvMatOrder* currMatOrder = env->jspMatOrderList;
    S32 currMatGroup = 0;

    xRenderStateZModesSet(TRUE, TRUE);

    if (alpha)
        currMatOrder += env->numOpaque;
    S32 matOrderEnd = alpha ? env->numTransparent : env->numOpaque;
    for (i = 0; i < matOrderEnd; i++, currMatOrder++) {
        if (iEnvIsJSPVisible(env, currMatOrder->jspIndex)) {
            RpAtomic* apAtom = currMatOrder->atomic;
            xJSPNodeInfo* nodeInfo = currMatOrder->nodeInfo;

            if (rwObjectGetFlags(apAtom) & rpATOMICRENDER) {
                RwFrame* frame = RpAtomicGetFrame(apAtom);

                if (!xModelSphereCull(iModelWorldBoundSphere(apAtom, &frame->ltm, NULL))) {
                    if (backcullon) {
                        if (nodeInfo->nodeFlags & 0x4) {
                            backcullon = FALSE;
                            currMatGroup = 0;
                            RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLNONE);
                        }
                    } else {
                        if (!(nodeInfo->nodeFlags & 0x4)) {
                            backcullon = TRUE;
                            currMatGroup = 0;
                            RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLBACK);
                        }
                    }

                    if (zbufferon) {
                        if (nodeInfo->nodeFlags & 0x2) {
                            zbufferon = FALSE;
                            currMatGroup = 0;
                            xRenderStateZModesSet(FALSE, TRUE);
                        }
                    } else {
                        if (!(nodeInfo->nodeFlags & 0x2)) {
                            zbufferon = TRUE;
                            currMatGroup = 0;
                            xRenderStateZModesSet(TRUE, TRUE);
                        }
                    }

                    U32 flagValue;
                    if (nodeInfo->nodeFlags & 0x2) {
                        flagValue = 0;
                    } else {
                        flagValue = (nodeInfo->nodeFlags >> 14) & 0x3;
                    }

                    U8 alphaDiscard;
                    switch (flagValue) {
                    case 0:
                        alphaDiscard = 0;
                        break;
                    case 1:
                        alphaDiscard = 100;
                        break;
                    case 2:
                        alphaDiscard = 150;
                        break;
                    case 3:
                        alphaDiscard = 200;
                        break;
                    }

                    if (!alphaDiscard) {
                        for (S32 i = 0; i < apAtom->geometry->matList.numMaterials; i++) {
                            RwTexture* texture = apAtom->geometry->matList.materials[i]->texture;
                            if (texture && texture->raster) {
                                RwGameCubeRasterExtension* rasExt =
                                    RwGameCubeRasterGetExtension(RwRasterGetParent(texture->raster));
                                if (rasExt->flags & 0x1)
                                    alphaDiscard = 1;
                            }
                        }
                    }

                    if (alphaDiscard) {
                        xRenderStateSetAlphaDiscard(alphaDiscard);
                    }

                    currMatGroup = currMatOrder->matGroup;
                    RpAtomicRender(apAtom);

                    if (alphaDiscard) {
                        xRenderStateResetAlphaDiscard();
                    }
                }
            }
        }
    }

    if (!backcullon)
        RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLBACK);
    if (!zbufferon) {
        xRenderStateZModesSet(TRUE, TRUE);
    }
}

void iEnvLightPreRender(iEnv* env, RpLight* light)
{
    for (S32 i = 0; i < env->jsp_count; i++) {
        if (env->jsp_visibilityCount[i] > 0)
            xJSP_ApplyLocalLight(env->jsp_list[i], light);
    }
}

void iEnvLightPostRender(iEnv* env)
{
    for (S32 i = 0; i < env->jsp_count; i++)
        xJSP_ClearLocalLight(env->jsp_list[i]);
}
