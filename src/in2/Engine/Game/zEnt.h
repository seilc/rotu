#ifndef ZENT_H
#define ZENT_H

#include "xEnt.h"

struct zEnt : xEnt
{
    xAnimTable* atbl;
};

xModelInstance* zEntRecurseModelInfo(void* info, xEnt* ent);

#endif
