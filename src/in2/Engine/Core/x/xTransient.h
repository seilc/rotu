#ifndef XTRANSIENT_H
#define XTRANSIENT_H

#include "xMath3.h"
#include "xSpacePartitionTree.h"

namespace xTransient {

extern xSpacePartitionTree2D space_tree;

void scene_enter(const xBox& world_box);
void scene_exit();
void reset();

inline void debug_update(F32 dt) {}
inline void debug_render() {}

}

#endif
