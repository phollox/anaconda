#ifndef CHOWDREN_NUMUPDOWN_H
#define CHOWDREN_NUMUPDOWN_H

#include "frameobject.h"

#ifdef CHOWDREN_USE_GWEN
#include "Gwen/Controls/NumericUpDown.h"
#endif

class NumericUpDown : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(NumericUpDown)

#ifdef CHOWDREN_USE_GWEN
    Gwen::Controls::NumericUpDown control;
#endif

    NumericUpDown(int x, int y, int type_id);
    ~NumericUpDown();
    void set_value(int value);
    int get_value();
    void init_control(int value, int min, int max);
    void update();
    void draw();
};

#endif // CHOWDREN_NUMUPDOWN_H
