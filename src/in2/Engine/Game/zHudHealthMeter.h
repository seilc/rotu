#ifndef ZHUDHEALTHMETER_H
#define ZHUDHEALTHMETER_H

#include "zHud.h"

namespace zHud {

class HealthMeter : public hud_element
{
public:
    HealthMeter();

    void SetCurValue(F32 value, bool defeated);
    void SetMaxValue(F32 value);

    virtual void reset();
    virtual void setup();
    virtual void destroy();
    virtual void update(F32);
    virtual void render();
    virtual void show();
    virtual void hide();

private:
    F32 m_fCurValue;
    F32 m_fMaxValue;
    U32 m_nCharacter;
};

} // namespace zHud

#endif
