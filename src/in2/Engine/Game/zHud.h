#ifndef ZHUD_H
#define ZHUD_H

#include "xBase.h"

namespace zHud {

class hud_element : public xBase
{
    hud_element* _next;

public:
    virtual void init();
    virtual void setup() {}
    virtual void update(F32) {}
    virtual void destroy() {}
    virtual void show() = 0;
    virtual void hide() = 0;
    virtual void handle_event(xBase*, U32, const F32*, xBase*, U32) {}
    virtual void render() {}

protected:
    char text[256];
};

} // namespace zHud

#endif
