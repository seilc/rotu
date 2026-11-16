#ifndef ZNPCFIREBALL_H
#define ZNPCFIREBALL_H

struct xEnt;
struct xVec3;

void zNPCFireballSetup();
void zNPCFireballLaunch(xEnt* owner, const xVec3& pos, const xVec3& dir, bool strong);

#endif
