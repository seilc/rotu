#ifndef ZHUD_H
#define ZHUD_H

#include "xBase.h"

namespace zHud {

class hud_element : public xBase
{
    friend class hud_system;

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

    hud_element* next()
    {
        return _next;
    }

protected:
    char text[256];
};

class hud_system
{
    hud_element* elements;
    S8 hide_count;
    bool last_paused;

    hud_system() {}

public:
    static void Startup();
    static void Shutdown();
    void init();
    void update(F32 dt);
    void render();
    void show();
    void hide();
    void add(hud_element* element);
    void destroy();
    static hud_system& instance();
};

} // namespace zHud

#endif
