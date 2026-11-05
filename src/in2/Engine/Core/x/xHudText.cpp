#include "xHudText.h"

#include "xHudAsset.h"
#include "zTextBox.h"
#include "zScene.h"
#include "xTextAsset.h"
#include "zMain.h"

#include <string.h>

namespace xhud {
    
void text_widget::load(xBase& data, xDynAsset& asset, size_t asset_size)
{
    init_base(data, asset, sizeof(widget_chunk));
    
    widget_chunk& wc = (widget_chunk&)data;
    new (&wc.w) text_widget((text_asset&)asset);
}

text_widget::text_widget(const text_asset& a)
    : widget(a)
{
    text[0] = '\0';
}

void text_widget::destruct() { widget::destruct(); }

void text_widget::setup()
{
    presetup();

    const text_asset& ta = *(text_asset*)a;
    ztextbox* ztb = (ztextbox*)zSceneFindObject(ta.text_box);

    if (ztb == 0) {
        tb = xtextbox::create();
        return;
    }
    
    ztb->refresh();
    tb = ztb->tb;
    ztb->get_text(text, sizeof(text));

    if (ta.text != 0) {
        U32 len;
        const char* s = xTextFindString(ta.text, &len);

        if (s) {
            memcpy(text, s, len);
            text[len] = '\0';
        }
    }

    rc.r = (U8)(tb.font.color.r / 255.0f);
    rc.g = (U8)(tb.font.color.g / 255.0f);
    rc.b = (U8)(tb.font.color.b / 255.0f);
    rc.a = (U8)(tb.font.color.a / 255.0f);

    start_rc.r = rc.r;
    start_rc.g = rc.g;
    start_rc.b = rc.b;
    start_rc.a = rc.a;
}

void text_widget::destroy()
{
    destruct();
}

U32 text_widget::type() const
{
    static U32 myid = xStrHash(text_asset::type_name());
    return myid;
}

bool text_widget::is(U32 id) const
{
    return id == text_widget::type() || widget::is(id);
}

void text_widget::update(F32 dt)
{
    updater(dt);

    tb.set_text(text);

    tb.bounds.x = rc.loc.x;
    tb.bounds.y = rc.loc.y;
    tb.bounds.w = rc.size.x;
    tb.bounds.h = rc.size.y;

    if (rc.size.x < 1.0f)
        tb.bounds.w = 640.0f * rc.size.x / FB_XRES;

    if (rc.size.y < 1.0f) {
        tb.bounds.h = 480.0f * rc.size.y / FB_YRES;
    }

    tb.font.clip = tb.bounds;
}

void text_widget::render()
{
    tb.render(true);
}

void text_widget::set_text(const char* intext)
{
    size_t textchars = strlen(intext);
    size_t copychars = MIN(textchars, sizeof(text) - 1);

    memcpy(text, intext, copychars);
    text[copychars] = '\0';
}

}
