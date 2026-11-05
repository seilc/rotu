#ifndef XDRAW_H
#define XDRAW_H

#include "xColor.h"
#include "xMath2.h"
#include "xMath3.h"

inline void xDrawSetColor(xColor color) {}
inline void xDrawSetColor(U8 r, U8 g, U8 b, U8 a) {}
inline void xDrawLine(const xVec3* a, const xVec3* b) {}
inline void xDrawArrow(const xVec3* from, const xVec3* dir, bool asRay, F32 length) {}
inline void xDrawSphere(const xSphere* sph, U32 flags) {}
inline void xDrawSphere(const xVec3* center, F32 r, U32 flags) {}
inline void xDrawCard(RwRaster* raster, const xFRect& src_rect, const xFRect& dst_rect, bool zwrite) {}

S32 Im2DRenderQuad(F32 x1, F32 y1, F32 x2, F32 y2, F32 z, F32 recipCamZ, F32 uvOffset);
S32 Im2DRenderQuadAlpha(F32 x1, F32 y1, F32 x2, F32 y2, F32 alpha, F32 z, F32 recipCamZ, F32 uvOffset);

#endif
