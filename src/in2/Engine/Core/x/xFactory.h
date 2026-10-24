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
    xFactory(S32 maxTypes, U32 memtag); // inferred from Incredibles GC, Rat MAP
    ~xFactory();
    S32 RegItemType(XGOFTypeInfo* info);
    S32 RegItemType(S32 tid, xFactoryCreateCallback create, xFactoryDestroyCallback destroy); // inferred from TSSM GC, Rat MAP
    void GrowDataEnable(xBase* user, U32 memtag, S32 isResume); // inferred from TSSM GC, Rat MAP
    void GrowDataDisable();
    xFactoryInst* CreateItem(S32 typeID, void* userdata, RyzMemGrow* callerzgrow);
    void DestroyAll();
    void DestroyItem(xFactoryInst* item);

protected:
    XGOFTypeInfo* infopool;
    st_XORDEREDARRAY infolist;
    xFactoryInst* products;
    RyzMemGrow growContextData;
};

#endif
