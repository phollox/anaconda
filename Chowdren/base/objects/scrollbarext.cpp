#include "objects/scrollbarext.h"
#include "chowconfig.h"
#include "Gwen/Controls/HorizontalScrollBar.h"
#include "Gwen/Controls/VerticalScrollBar.h"

// ScrollbarObject

ScrollbarObject::ScrollbarObject(int x, int y, int type_id)
: FrameObject(x, y, type_id), scrollbar(NULL)
{
    flags &= ~SCROLL;
    collision = new InstanceBox(this);
}

ScrollbarObject::~ScrollbarObject()
{
    delete scrollbar;
    delete collision;
}

void ScrollbarObject::init_scrollbar(int value)
{
    if (vertical)
        scrollbar = new Gwen::Controls::VerticalScrollBar(gwen.canvas);
    else
        scrollbar = new Gwen::Controls::HorizontalScrollBar(gwen.canvas);

    scrollbar->SetPos(x, y);
    scrollbar->SetSize(width, height);
    set_scroll_range(min_val, max_val);

    float range = (float)(max_val - min_val);
    if (range != 0.0f) {
        float clamped_val = (value - min_val) / range;
        scrollbar->SetScrolledAmount(clamped_val, true);
    }
}

void ScrollbarObject::set_width(int width)
{
    this->width = std::max(0, width);
    collision->update_aabb();
    scrollbar->SetWidth(this->width);
}

void ScrollbarObject::set_height(int height)
{
    this->height = std::max(0, height);
    collision->update_aabb();
    scrollbar->SetHeight(this->height);
}

void ScrollbarObject::set_scroll_range(int min, int max)
{
    min_val = min;
    max_val = std::max(min, max);

    float content_size = (float)(max_val - min_val);
    scrollbar->SetContentSize(content_size);
    // This determines the grip size. Try using the scrollbar size as reference,
    // but have a third of the entire scroll bar height as maximum grip size.
    float bar_size = (float)(vertical ? height : width);
    scrollbar->SetViewableContentSize(std::min(bar_size, content_size / 3.f));
}

int ScrollbarObject::get_value()
{
    return min_val + scrollbar->GetScrolledAmount() * (max_val - min_val);
}


void ScrollbarObject::update()
{
    scrollbar->SetPos(x, y);
}

void ScrollbarObject::draw()
{
    gwen.render(scrollbar);
    // In case of emergency:
    //gwen.canvas->RenderCanvas();
}