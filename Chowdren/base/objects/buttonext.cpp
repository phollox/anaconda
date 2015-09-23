#include "objects/buttonext.h"
#include "collision.h"
#include "manager.h"

// ButtonObject

#ifdef CHOWDREN_USE_GWEN
#include "gui/gwen.h"
#include "Gwen/Controls/CheckBox.h"

class Button : public Gwen::Controls::Button
{
public:
    ButtonObject * parent;

    GWEN_CONTROL_INLINE(Button, Gwen::Controls::Button)
    {
    }

    void OnPress()
    {
        parent->clicked = 2;
        Gwen::Controls::Button::OnPress();
    }
};

class CheckBox : public Gwen::Controls::CheckBox
{
public:
    ButtonObject * parent;

    GWEN_CONTROL_INLINE(CheckBox, Gwen::Controls::CheckBox)
    {
    }

    void OnPress()
    {
        parent->clicked = 2;
        Gwen::Controls::CheckBox::OnPress();
    }
};

#endif

ButtonObject::ButtonObject(int x, int y, int type_id)
: FrameObject(x, y, type_id)
{
#ifdef CHOWDREN_USE_GWEN
    collision = new InstanceBox(this);
    clicked = 0;
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
    if (flags & IS_CHECKBOX) {
        button = new CheckBox(manager.frame->gwen.frame_base);
        ((CheckBox*)button)->parent = this;
    } else {
        button = new Button(manager.frame->gwen.frame_base);
        ((Button*)button)->parent = this;
    }
    button->SetPos(x, y);
    button->SetSize(width, height);
#endif
}

void ButtonObject::update()
{
#ifdef CHOWDREN_USE_GWEN
    button->SetHidden(!get_visible());
    button->SetPos(x, y);
    button->SetSize(width, height);
    clicked = std::max(clicked - 1, 0);
#endif
}

void ButtonObject::draw()
{
#ifdef CHOWDREN_USE_GWEN
    button->SetHidden(!get_visible());
    button->SetPos(x, y);
    button->SetSize(width, height);
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

void ButtonObject::set_text(const std::string & text)
{
#ifdef CHOWDREN_USE_GWEN
    button->SetText(Gwen::TextObject(text));
#endif
}

void ButtonObject::enable()
{
#ifdef CHOWDREN_USE_GWEN
    button->SetDisabled(false);
#endif
}

void ButtonObject::disable()
{
#ifdef CHOWDREN_USE_GWEN
    button->SetDisabled(true);
#endif
}

bool ButtonObject::is_clicked()
{
#ifdef CHOWDREN_USE_GWEN
    return clicked > 0;
#else
    return false;
#endif
}