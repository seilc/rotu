#ifndef ZHUDEXPERIENCEPOINTMETER_H
#define ZHUDEXPERIENCEPOINTMETER_H

#include "zHud.h"

namespace zHud {

class ExperiencePointMeter : public hud_element
{
public:
    ExperiencePointMeter();

    void SetExpPoints(S32 points);
    void SetExpPointsToUpgrade(S32 points);

    virtual void setup();
    virtual void update(F32);
    virtual void reset();
    virtual void show();
    virtual void hide();
    virtual void destroy();
    virtual void render();

private:
    S32 m_nExpPoints;
    S32 m_nExpPointsToUpgrade;
    U32 m_nCharacter;
};

} // namespace zHud

#endif
