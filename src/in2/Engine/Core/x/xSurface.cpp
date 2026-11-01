#include "xSurface.h"

#include "xMemMgr.h"

#include "decomp.h"

static xSurface* surfs;
static U16 nsurfs;

void xSurfaceInit(U16 num_surfs)
{
    nsurfs = num_surfs;

    if (nsurfs) {
        surfs = (xSurface*)xMEMALLOC(nsurfs * sizeof(xSurface), 0);

        for (U16 i = 0; i < nsurfs; i++) {
            surfs[i].idx = i;
        }
    } else {
        surfs = NULL;
    }
}

DECOMP_FORCEACTIVE(&xBase::operator=)

void xSurfaceExit()
{
}

void xSurfaceSave(xSurface* ent, xSerial* s)
{
    xBaseSave(ent, s);
}

void xSurfaceLoad(xSurface* ent, xSerial* s)
{
    xBaseLoad(ent, s);
}

void xSurfaceReset(xSurface*)
{
}

U16 xSurfaceGetNumSurfaces()
{
    return nsurfs;
}

xSurface* xSurfaceGetByIdx(U16 n)
{
    if (!surfs) {
        return NULL;
    }

    return &surfs[n];
}
