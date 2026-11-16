#include "xCommon.h"
#include "zUIUVs.h"

F32* zUIUVs_Init(RpAtomic* atomic, U32& texCoordsCount)
{
    F32* uvCoords = NULL;
    texCoordsCount = 0;

    RpGeometry* geom = RpAtomicGetGeometry(atomic);

    if (geom) {
        RwTexCoords* c = RpGeometryGetVertexTexCoords(geom, rwTEXTURECOORDINATEINDEX0);

        if (c) {
            S32 numcoordsets = RpGeometryGetNumTexCoordSets(geom);
            S32 i, numVerts = RpGeometryGetNumVertices(geom);

            texCoordsCount = numVerts * 2;
            uvCoords = new (eMemStaticType_0, eMemMgrTag_HUD) F32[texCoordsCount];

            F32* uv = uvCoords;
            RpGeometryLock(geom, rpGEOMETRYLOCKTEXCOORDS1);
            for (i = 0; i < numVerts; i++) {
                *uv++ = c->u;
                *uv++ = c->v;
                c++;
            }
            RpGeometryUnlock(geom);
        }
    }

    return uvCoords;
}

void zUIUVs_Update(RpAtomic* atomic, F32 value, const F32* uvCoords, bool, bool update_v, S32 type, F32 offset_u)
{
    RpGeometry* geom = RpAtomicGetGeometry(atomic);

    if (geom) {
        RwTexCoords* c = RpGeometryGetVertexTexCoords(geom, rwTEXTURECOORDINATEINDEX0);

        if (c) {
            S32 numcoordsets = RpGeometryGetNumTexCoordSets(geom), i, numVerts;
            const F32* uv = uvCoords;

            RpGeometryLock(geom, rpGEOMETRYLOCKTEXCOORDS1);
            numVerts = RpGeometryGetNumVertices(geom);
            for (i = 0; i < numVerts; i++) {
                c->u = uv[0] + offset_u;
                if (update_v) {
                    switch (type) {
                    case 1:
                        c->v = uv[1] - 0.5f * (1.0f - value);
                        break;
                    case 2:
                        c->v = uv[1] - 0.5f * (1.0f - value);
                        break;
                    }
                }
                uv += 2;
                c++;
            }
            RpGeometryUnlock(geom);
        }
    }
}

void zUIUVs_Reset(RpAtomic* atomic, const F32* uvCoords)
{
    RpGeometry* geom = RpAtomicGetGeometry(atomic);

    if (geom) {
        RwTexCoords* c = RpGeometryGetVertexTexCoords(geom, rwTEXTURECOORDINATEINDEX0);

        if (c) {
            S32 numcoordsets = RpGeometryGetNumTexCoordSets(geom), i, numVerts;
            const F32* uv = uvCoords;

            RpGeometryLock(geom, rpGEOMETRYLOCKTEXCOORDS1);
            numVerts = RpGeometryGetNumVertices(geom);
            for (i = 0; i < numVerts; i++) {
                c->u = uv[0];
                c->v = uv[1];
                uv += 2;
                c++;
            }
            RpGeometryUnlock(geom);
        }
    }
}
