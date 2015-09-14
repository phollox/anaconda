#ifndef CHOWDREN_BUTTONEXT_H
#define CHOWDREN_BUTTONEXT_H

#include "frameobject.h"

#ifdef CHOWDREN_USE_GWEN
#include "Gwen/Controls/Button.h"
#endif

class ButtonObject : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(ButtonObject)

    enum {
        IS_CHECKBOX = 1 << 0
    };

#ifdef CHOWDREN_USE_GWEN
    unsigned int button_flags;
    Gwen::Controls::Button * button;
#endif

    ButtonObject(int x, int y, int type_id);
    ~ButtonObject();
    void init_button(unsigned int flags);
    void update();
    void draw();
    void check();
    void uncheck();
};

#endif // CHOWDREN_BUTTONEXT_H
