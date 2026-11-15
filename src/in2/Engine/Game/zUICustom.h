#ifndef ZUICUSTOM_H
#define ZUICUSTOM_H

#include "types.h"

class zUI;

class zUICustom
{
public:
    virtual void Init();

    zUI* GetUI() const
    {
        return ui;
    }

protected:
    zUI* ui;
};

void zSendUIEvent(const char* widgetName, U32 event);

#endif
