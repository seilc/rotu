#ifndef XDEBUGGRAPH_H
#define XDEBUGGRAPH_H

#include "types.h"

void xDebugGraphSceneEnter();
void xDebugGraphSceneExit();
void xDebugGraphUpdate(F32 dt); // UNUSED
void xDebugGraphRender();
void xDebugGraphSetScale(const char* name, F32 scale); // UNUSED
void xDebugGraphSetOriginValue(const char* name, F32 originValue); // UNUSED
void xDebugGraphAdd(const char* name, F32 value); // UNUSED
void xDebugGraphAdd(const char* name, S32 value); // UNUSED
void xDebugGraphBind(const char* name, const F32* value, F32 update_delay); // UNUSED

#endif
