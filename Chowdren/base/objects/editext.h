#ifndef CHOWDREN_EDITEXT_H
#define CHOWDREN_EDITEXT_H

#include "frameobject.h"

#ifdef CHOWDREN_USE_EDITOBJ
#include "collision.h"
#include "common.h"
#include "font.h"
#include <string>
#endif

#ifdef CHOWDREN_USE_GWEN
#include "Gwen/Controls/TextBox.h"
#endif

class EditObject : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(EditObject)

    enum EditFlags
    {
        PASSWORD = 1 << 0,
        FOCUS = 1 << 1,
        READ_ONLY = 1 << 2,
        MULTILINE = 1 << 3,
    };

    EditObject(int x, int y, int type_id);
    void set_text(const std::string & value);
    const std::string & get_text();
    bool get_focus();
    void enable_focus();
    void disable_focus();
    void set_limit(int size);
    void disable();
    void scroll_to_end();

#ifdef CHOWDREN_USE_EDITOBJ
    int edit_flags;
    InstanceBox edit_col;
    std::string text;
    FTTextureFont * font;
    int limit;

#ifdef CHOWDREN_USE_GWEN
    Gwen::Controls::TextBox * text_box;
    Gwen::Controls::ScrollControl * scroller;
    Gwen::Controls::Base * base_control;
    std::string new_text;

    void init_control();
    void update_text();
#endif

    ~EditObject();
    void update();
    void draw();
#endif
};

#endif // CHOWDREN_EDITEXT_H
