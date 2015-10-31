// Copyright (c) Mathias Kaerlev 2012-2015.
//
// This file is part of Anaconda.
//
// Anaconda is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Anaconda is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Anaconda.  If not, see <http://www.gnu.org/licenses/>.

#include "objects/systembox.h"
#include "include_gl.h"
#include "collision.h"
#include <iostream>
#include "render.h"
#include "common.h"

#ifdef CHOWDREN_USE_GWEN
#include "gui/gwen.h"

class BoxButton : public Gwen::Controls::Button
{
public:
    SystemBox * parent;

	GWEN_CONTROL_INLINE(BoxButton, Gwen::Controls::Button)
    {
    }

    void OnPress()
    {
        parent->clicked = 2;
        Gwen::Controls::Button::OnPress();
    }
};
#endif

SystemBox::SystemBox(int x, int y, int type_id)
: FrameObject(x, y, type_id), layout(NULL), box_flags(0)
{
    collision = new InstanceBox(this);
#ifdef CHOWDREN_USE_GWEN
    button = NULL;
    clicked = 0;
#endif
}

SystemBox::~SystemBox()
{
    delete collision;

#ifdef CHOWDREN_USE_GWEN
    if (button)
        button->DelayedDelete();
#endif
}

void SystemBox::draw()
{
#ifdef CHOWDREN_USE_GWEN
    if (button != NULL) {
        button->SetHidden(!get_visible());
        button->SetPos(x - frame->off_x, y - frame->off_y);
        button->SetSize(width, height);
        return;
    }
#endif

    if (image == NULL) {
        int x1 = x;
        int y1 = y;
        int x2 = x1 + width;
        int y2 = y1 + height;
        Render::draw_quad(x1, y1, x2, y2, Color(0, 0, 0, box_color.a));
        x1++;
        y1++;
        x2--;
        y2--;
        Render::draw_quad(x1, y1, x2, y2, box_color);

        FTTextureFont * font;
        if (layout == NULL) {
            if (!init_font())
                return;
            layout = new FTSimpleLayout();
            font = get_font(12);
            layout->SetFont(font);
            layout->SetAlignment(
                (TextAlignment)(box_flags & ALIGN_FLAGS_MASK));
#ifdef CHOWDREN_USE_GWEN
            layout->SetLineLength(width - margin[0] - margin[2]);
#else
            layout->SetLineLength(width);
#endif
        } else {
            font = get_font(12);
        }

        double off_y = y + font->Ascender();

        FTBBox bb = layout->BBoxL(text.c_str(), -1);
        double box_h = bb.Upper().Y() - bb.Lower().Y(); 
#ifdef CHOWDREN_USE_GWEN
        off_y += ((height - margin[1] - margin[3]) - box_h) * 0.5;
        off_y += margin[1];
        int off_x = x + margin[0];
#else
        off_y += (height - box_h) * 0.5;
        int off_x = x;
#endif

        FTTextureFont::color = blend_color;
        layout->Render(text.c_str(), -1, FTPoint(off_x, int(off_y)));
        // std::cout << "Draw system text: " << text << " " << name << std::endl;
        return;
    }
    int xx, yy;
    switch (type) {
        case PATTERN_IMAGE: {
            // XXX use Render::offset for offset here?
            int screen_x1 = 0 - (layer->off_x - frame->off_x);
            int screen_y1 = 0 - (layer->off_y - frame->off_y);
            int screen_x2 = screen_x1 + WINDOW_WIDTH;
            int screen_y2 = screen_y1 + WINDOW_HEIGHT;

            int x1 = x;
            int y1 = y;
            int x2 = x + width;
            int y2 = y + height;

            int xx1, yy1, xx2, yy2;
            intersect(screen_x1, screen_y1, screen_x2, screen_y2,
                      x1, y1, x2, y2,
                      xx1, yy1, xx2, yy2);

            x1 += ((xx1 - x1) / image->width) * image->width;
            y1 += ((yy1 - y1) / image->height) * image->height;
            x2 += ((xx2 - x2) / image->width) * image->width;
            y2 += ((yy2 - y2) / image->height) * image->height;

            Render::enable_scissor(x, y, width, height);
            for (xx = x1; xx < x2; xx += image->width)
            for (yy = y1; yy < y2; yy += image->height) {
                draw_image(image, xx + image->hotspot_x,
                           yy + image->hotspot_y, Color());
            }
			Render::disable_scissor();
            break;
        }
        case CENTER_IMAGE:
            xx = x + width / 2 - image->width / 2;
            yy = y + height / 2 - image->height / 2;
            draw_image(image, xx + image->hotspot_x, yy + image->hotspot_y,
                       Color());
            break;
        case TOPLEFT_IMAGE:
            draw_image(image, x + image->hotspot_x, y + image->hotspot_y,
                       Color());
            break;
        default:
            break;
    }
}

void SystemBox::hide_border_1()
{
    // std::cout << "Hide System Box border 1 not implemented" << std::endl;
}

void SystemBox::hide_border_2()
{
    // std::cout << "Hide System Box border 1 not implemented" << std::endl;
}

void SystemBox::hide_fill()
{
    // std::cout << "Hide System Box fill not implemented" << std::endl;
}

void SystemBox::set_size(int w, int h)
{
    width = w;
    height = h;
    collision->update_aabb();
    if (layout == NULL)
        return;
    layout->SetLineLength(width);
}

void SystemBox::set_text(const std::string & text)
{
#ifdef CHOWDREN_USE_GWEN
    if (button != NULL) {
        button->SetText(Gwen::TextObject(text));
    }
#endif
    this->text = text;
}

void SystemBox::set_border_1(Color color)
{

}

void SystemBox::set_border_2(Color color)
{

}

void SystemBox::set_fill(Color color)
{
    box_color = color;
}

void SystemBox::check()
{
    box_flags |= CHECKED;
}

void SystemBox::uncheck()
{
    box_flags &= ~CHECKED;
}

void SystemBox::disable()
{
    std::cout << "SystemBox::disable not implemented" << std::endl;
}

bool SystemBox::is_clicked()
{
#ifdef CHOWDREN_USE_GWEN
    return clicked > 0;
#else
    return mouse_over() && is_mouse_pressed_once(SDL_BUTTON_LEFT);
#endif
}

#ifdef CHOWDREN_USE_GWEN

void SystemBox::update()
{
    if (button == NULL)
        return;
    clicked = std::max(0, clicked - 1);
}

void SystemBox::init_button()
{
    flags &= ~SCROLL;
    button = new BoxButton(manager.frame->gwen.frame_base);
    ((BoxButton*)button)->parent = this;
}

#endif

const std::string & SystemBox::get_font_name()
{
    return empty_string;
}

class DefaultInstance : public SystemBox
{
public:
    DefaultInstance()
    : SystemBox(0, 0, 0)
    {
        collision = new InstanceBox(this);
        create_alterables();
        setup_default_instance(this);
    }
};

static DefaultInstance default_systembox;
FrameObject * default_systembox_instance = &default_systembox;
