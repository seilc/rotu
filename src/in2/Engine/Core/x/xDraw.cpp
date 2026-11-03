#include "xDraw.h"

#include "xutil.h"

S32 Im2DRenderQuad(F32 x1, F32 y1, F32 x2, F32 y2, F32 z, F32 recipCamZ, F32 uvOffset)
{
    RwIm2DVertex vx[4];

    RwIm2DVertexSetScreenX(&vx[0], x1);
    RwIm2DVertexSetScreenY(&vx[0], y1);
    RwIm2DVertexSetScreenZ(&vx[0], z);
    RwIm2DVertexSetIntRGBA(&vx[0], 255, 255, 255, 255);
    RwIm2DVertexSetU(&vx[0], uvOffset, recipCamZ);
    RwIm2DVertexSetV(&vx[0], uvOffset, recipCamZ);

    RwIm2DVertexSetScreenX(&vx[1], x1);
    RwIm2DVertexSetScreenY(&vx[1], y2);
    RwIm2DVertexSetScreenZ(&vx[1], z);
    RwIm2DVertexSetIntRGBA(&vx[1], 255, 255, 255, 255);
    RwIm2DVertexSetU(&vx[1], uvOffset, recipCamZ);
    RwIm2DVertexSetV(&vx[1], 1.0f + uvOffset, recipCamZ);

    RwIm2DVertexSetScreenX(&vx[2], x2);
    RwIm2DVertexSetScreenY(&vx[2], y1);
    RwIm2DVertexSetScreenZ(&vx[2], z);
    RwIm2DVertexSetIntRGBA(&vx[2], 255, 255, 255, 255);
    RwIm2DVertexSetU(&vx[2], 1.0f + uvOffset, recipCamZ);
    RwIm2DVertexSetV(&vx[2], uvOffset, recipCamZ);

    RwIm2DVertexSetScreenX(&vx[3], x2);
    RwIm2DVertexSetScreenY(&vx[3], y2);
    RwIm2DVertexSetScreenZ(&vx[3], z);
    RwIm2DVertexSetIntRGBA(&vx[3], 255, 255, 255, 255);
    RwIm2DVertexSetU(&vx[3], 1.0f + uvOffset, recipCamZ);
    RwIm2DVertexSetV(&vx[3], 1.0f + uvOffset, recipCamZ);

    RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, vx, ARRAY_LENGTH(vx));

    return TRUE;
}

S32 Im2DRenderQuadAlpha(F32 x1, F32 y1, F32 x2, F32 y2, F32 alpha, F32 z, F32 recipCamZ, F32 uvOffset)
{
    RwIm2DVertex vx[4];
    U8 alphaI = 255.0f * alpha;

    RwIm2DVertexSetScreenX(&vx[0], x1);
    RwIm2DVertexSetScreenY(&vx[0], y1);
    RwIm2DVertexSetScreenZ(&vx[0], z);
    RwIm2DVertexSetIntRGBA(&vx[0], 255, 255, 255, alphaI);
    RwIm2DVertexSetU(&vx[0], uvOffset, recipCamZ);
    RwIm2DVertexSetV(&vx[0], uvOffset, recipCamZ);

    RwIm2DVertexSetScreenX(&vx[1], x1);
    RwIm2DVertexSetScreenY(&vx[1], y2);
    RwIm2DVertexSetScreenZ(&vx[1], z);
    RwIm2DVertexSetIntRGBA(&vx[1], 255, 255, 255, alphaI);
    RwIm2DVertexSetU(&vx[1], uvOffset, recipCamZ);
    RwIm2DVertexSetV(&vx[1], 1.0f + uvOffset, recipCamZ);

    RwIm2DVertexSetScreenX(&vx[2], x2);
    RwIm2DVertexSetScreenY(&vx[2], y1);
    RwIm2DVertexSetScreenZ(&vx[2], z);
    RwIm2DVertexSetIntRGBA(&vx[2], 255, 255, 255, alphaI);
    RwIm2DVertexSetU(&vx[2], 1.0f + uvOffset, recipCamZ);
    RwIm2DVertexSetV(&vx[2], uvOffset, recipCamZ);

    RwIm2DVertexSetScreenX(&vx[3], x2);
    RwIm2DVertexSetScreenY(&vx[3], y2);
    RwIm2DVertexSetScreenZ(&vx[3], z);
    RwIm2DVertexSetIntRGBA(&vx[3], 255, 255, 255, alphaI);
    RwIm2DVertexSetU(&vx[3], 1.0f + uvOffset, recipCamZ);
    RwIm2DVertexSetV(&vx[3], 1.0f + uvOffset, recipCamZ);

    RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, vx, ARRAY_LENGTH(vx));

    return TRUE;
}
