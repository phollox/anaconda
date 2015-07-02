#ifndef CHOWDREN_SCROLLBAREXT_H
#define CHOWDREN_SCROLLBAREXT_H

#include "frameobject.h"
#include "common.h"

#include "Gwen/Gwen.h"
#include "Gwen/Controls/ScrollBar.h"
#include "gui/gwen.h"

class ScrollbarObject : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(ScrollbarObject)

    Gwen::Controls::BaseScrollBar* scrollbar;

    bool vertical;
    int min_val;
    int max_val;

    ScrollbarObject(int x, int y, int type_id);
    ~ScrollbarObject();

    void set_width(int width);
    void set_height(int height);
    void set_scroll_range(int min, int max);
    int get_value();

    void init_scrollbar(int value);

    void update();
    void draw();
};

#endif // CHOWDREN_SCROLLBAREXT_H
