#include "objects/scrollbarext.h"
#include "chowconfig.h"

// ScrollbarObject

ScrollbarObject::ScrollbarObject(int x, int y, int type_id)
: FrameObject(x, y, type_id), focus(false), dragging(false)
{
    flags &= ~SCROLL;
    collision = new InstanceBox(this);
}

ScrollbarObject::~ScrollbarObject()
{
    delete collision;
}

// Helpers

int ScrollbarObject::get_total_size()
{
    return vertical ? height : width;
}

int ScrollbarObject::get_inner_size()
{
    return get_total_size() - 2 * button_size;
}

int ScrollbarObject::get_grip_size()
{
    // Dynamic grip size not worth it
    return 25;
}

int ScrollbarObject::get_track_size()
{
    return get_inner_size() - get_grip_size();
}

int ScrollbarObject::pos_to_val(int pos)
{
    float v = min_val + (max_val - min_val) * ((float)pos / get_track_size());
    return (int)floor(v + 0.5f);
}

int ScrollbarObject::get_grip_pos()
{
    float scroll_rate = (float)(val - min_val) / max_val;
    float pos = scroll_rate * get_track_size();
    return (int)floor(pos + 0.5f);
}

// ACE

void ScrollbarObject::set_width(int width)
{
    this->width = std::max(0, width);
    collision->update_aabb();
}

void ScrollbarObject::set_height(int height)
{
    this->height = std::max(0, height);
    collision->update_aabb();
}

void ScrollbarObject::set_scroll_range(int min, int max)
{
    min_val = min;
    max_val = max;
    val = std::max(min_val, std::min(max_val, val));
}

int ScrollbarObject::get_value()
{
    // While dragging, the new value isn't written yet
    if (dragging)
        return pos_to_val(drag_new_pos);
    return val;
}


void ScrollbarObject::update()
{
    int mx, my;
    frame->get_mouse_pos(&mx, &my);

    int pos = vertical ? y : x;
    int mouse_pos = (vertical ? my : mx) - pos;
        
    if (is_mouse_pressed_once(SDL_BUTTON_LEFT)) {
        PointCollision col(mx, my);
        focus = collide(&col, collision);

        if (!focus)
            return;

        // Clicked on - arrow
        if (mouse_pos < button_size)
            val = std::max(min_val, val - 1);

        // Clicked on + arrow
        if (mouse_pos >= get_total_size() - button_size)
            val = std::min(max_val, val + 1);

        // Clicked on grip
        int grip_total_pos = get_grip_pos() + button_size;
        if (mouse_pos >= grip_total_pos
         && mouse_pos < grip_total_pos + get_grip_size())
        {
            dragging = true;
            drag_click_pos = mouse_pos;
        }
    }

    // The grip was clicked and is now being dragged
    if (dragging) {
        // When released, apply current position
        if (!is_mouse_pressed(SDL_BUTTON_LEFT)) {
            val = pos_to_val(drag_new_pos);
            dragging = false;
            return;
        }

        int delta = mouse_pos - drag_click_pos;
        drag_new_pos = get_grip_pos() + delta;
        drag_new_pos = std::max(0, std::min(get_track_size(), drag_new_pos));
    }
}


void ScrollbarObject::draw()
{
    static const Color background_color(204, 206, 217, 255);
    static const Color border_color(106, 107, 110, 255);
    static const Color button_color(240, 240, 240, 255);
    draw_box(0, get_total_size(), background_color, border_color, 1);

    draw_box(0, button_size, button_color, border_color, 1);
    draw_arrow(button_size / 2, true);
    
    draw_box(get_total_size() - button_size, button_size,
        button_color, border_color, 1);
    draw_arrow(get_total_size() - button_size / 2, false);

    int grip_pos = (dragging ? drag_new_pos : get_grip_pos()) + button_size;
    draw_box(grip_pos, get_grip_size(), button_color, border_color, 1);
}

void ScrollbarObject::draw_box(int pos, int size, const Color& fill,
    const Color& outline, int thickness)
{
    float x1, y1, x2, y2;
    if (vertical) {
        x1 = x;
        y1 = y + pos;
        x2 = x + width;
        y2 = y + pos + size;
    } else {
        x1 = x + pos;
        y1 = y;
        x2 = x + pos + size;
        y2 = y + height;
    }
    Render::draw_quad(x1, y1, x2, y2, outline);
    Render::draw_quad(x1 + thickness, y1 + thickness,
        x2 - thickness, y2 - thickness, fill);
}

void ScrollbarObject::draw_arrow(int pos, bool flip)
{
    int x = this->x + (vertical ? (width / 2) : pos);
    int y = this->y + (vertical ? pos : (height / 2));

    float points[] = {
        x - 4, y - 4,
        x + 4, y - 4,
        x + 4, y + 4,
        x - 4, y + 4,
    };
    
    int off;
    if (vertical) {
        off = flip ? 0 : 4;
        points[off] = x;
        points[off + 2] = x;
    } else {
        off = flip ? 7 : 3;
        points[off] = y;
        points[(off + 2) % 8] = y;
    }

    Render::draw_quad(&points[0], Color(64, 64, 64, 255));
}