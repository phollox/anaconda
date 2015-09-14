#include "objects/buttonext.h"
#include "collision.h"
#include "gui/gwen.h"
#include "Gwen/Controls/CheckBox.h"

// ButtonObject

ButtonObject::ButtonObject(int x, int y, int type_id)
: FrameObject(x, y, type_id)
{
#ifdef CHOWDREN_USE_GWEN
    collision = new InstanceBox(this);
#endif
}

ButtonObject::~ButtonObject()
{
#ifdef CHOWDREN_USE_GWEN
    delete collision;
    delete button;
#endif
}

void ButtonObject::init_button(unsigned int flags)
{
#ifdef CHOWDREN_USE_GWEN
    button_flags = flags;
    if (flags & IS_CHECKBOX)
        button = new Gwen::Controls::CheckBox(gwen.canvas);
    else
        button = new Gwen::Controls::Button(gwen.canvas);
    button->SetPos(x, y);
    button->SetSize(width, height);
#endif
}

void ButtonObject::update()
{
#ifdef CHOWDREN_USE_GWEN
    button->SetPos(x, y);
    button->SetSize(width, height);
#endif
}

void ButtonObject::draw()
{
#ifdef CHOWDREN_USE_GWEN
    gwen.render(button);
#endif
}

void ButtonObject::check()
{
#ifdef CHOWDREN_USE_GWEN
    if (!(flags & IS_CHECKBOX))
        return;
    ((Gwen::Controls::CheckBox*)button)->SetChecked(true);
#endif
}

void ButtonObject::uncheck()
{
#ifdef CHOWDREN_USE_GWEN
    if (!(flags & IS_CHECKBOX))
        return;
    ((Gwen::Controls::CheckBox*)button)->SetChecked(false);
#endif
}
