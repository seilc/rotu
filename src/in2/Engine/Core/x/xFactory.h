#ifndef XFACTORY_H
#define XFACTORY_H

#include "xRMemData.h"
#include "xordarray.h"

struct xFactoryInst : RyzMemData
{
protected:
    S32 itemType;
    xFactoryInst* nextprod;
    xFactoryInst* prevprod;

    friend struct xFactory;
};

typedef xFactoryInst* (*xFactoryCreateCallback)(S32, RyzMemGrow*, void*);
typedef void (*xFactoryDestroyCallback)(xFactoryInst*);

struct XGOFTypeInfo
{
    S32 tid;
    xFactoryCreateCallback creator;
    xFactoryDestroyCallback destroyer;
};

struct xFactory : RyzMemData
{
    xFactoryInst* CreateItem(S32 typeID, void* userdata, RyzMemGrow* callerzgrow); // UNUSED

protected:
    XGOFTypeInfo* infopool;
    st_XORDEREDARRAY infolist;
    xFactoryInst* products;
    RyzMemGrow growContextData;
};

#endif
