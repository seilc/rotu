#ifndef ZHUDSUPERMOVEVIALS_H
#define ZHUDSUPERMOVEVIALS_H

#include "zHud.h"

namespace zHud {

class SuperMoveVials : public hud_element
{
public:
    SuperMoveVials();

    virtual void setup();
    virtual void update(F32);
    virtual void reset();
    virtual void show();
    virtual void hide();
    virtual void destroy();
    virtual void render();

private:
    S32 m_nCurVials;
    S32 m_nMaxVials;
    S32 m_nState;
};

} // namespace zHud

#endif
