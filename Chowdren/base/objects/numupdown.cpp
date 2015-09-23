#include "numupdown.h"
#include "gui/gwen.h"
#include "collision.h"
#include "manager.h"

// NumericUpDown

NumericUpDown::NumericUpDown(int x, int y, int type_id)
: FrameObject(x, y, type_id)
#ifdef CHOWDREN_USE_GWEN
, control(manager.frame->gwen.frame_base)
#endif
{
#ifdef CHOWDREN_USE_GWEN
    collision = new InstanceBox(this);
#endif
}

NumericUpDown::~NumericUpDown()
{
#ifdef CHOWDREN_USE_GWEN
    delete collision;
#endif
}

void NumericUpDown::init_control(int value, int min, int max)
{
#ifdef CHOWDREN_USE_GWEN
    control.SetMin(min);
    control.SetMax(max);
    control.SetValue(value);
    control.SetPos(x, y);
    control.SetSize(width, height);
#endif
}

void NumericUpDown::update()
{
#ifdef CHOWDREN_USE_GWEN
    control.SetHidden(!get_visible());
    control.SetPos(x, y);
    control.SetSize(width, height);
#endif
}

void NumericUpDown::draw()
{
#ifdef CHOWDREN_USE_GWEN
    control.SetHidden(!get_visible());
    control.SetPos(x, y);
    control.SetSize(width, height);
#endif
}

void NumericUpDown::set_value(int value)
{
#ifdef CHOWDREN_USE_GWEN
    control.SetValue(value);
#endif
}

int NumericUpDown::get_value()
{
#ifdef CHOWDREN_USE_GWEN
    return control.GetIntValue();
#endif
}