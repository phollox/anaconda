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

#include "objects/editext.h"

#ifdef CHOWDREN_USE_GWEN
#include "gui/gwen.h"
#endif

// EditObject

#ifndef CHOWDREN_USE_EDITOBJ

EditObject::EditObject(int x, int y, int type_id)
: FrameObject(x, y, type_id)
{
}

void EditObject::set_text(const std::string & value)
{

}

const std::string & EditObject::get_text()
{
    return empty_string;
}

bool EditObject::get_focus()
{
    return true;
}

void EditObject::set_limit(int size)
{
    std::cout << "Edit: set_limit not implemented" << std::endl;
}

void EditObject::disable_focus()
{
    std::cout << "Edit: disable_focus not implemented" << std::endl;
}


void EditObject::enable_focus()
{
    std::cout << "Edit: enable_focus not implemented" << std::endl;
}

#else

EditObject::EditObject(int x, int y, int type_id)
: FrameObject(x, y, type_id), edit_flags(0), edit_col(this),
  font(get_font(14)), limit(-1)
{
    collision = &edit_col;
}

#ifdef CHOWDREN_USE_GWEN
class TextScroller : public Gwen::Controls::ScrollControl
{
public:
    GWEN_CONTROL_INLINE(TextScroller, Gwen::Controls::ScrollControl)
    {

    }
};

void EditObject::init_control()
{
    flags &= ~SCROLL;
    Gwen::Controls::Base * base = manager.frame->gwen.frame_base;
    if ((edit_flags & (MULTILINE | READ_ONLY)) == (MULTILINE | READ_ONLY)) {
        scroller = new Gwen::Controls::ScrollControl(base);
        scroller->SetScroll(false, true);
        text_box = new Gwen::Controls::TextBoxMultiline(scroller);
        base_control = scroller;
    } else if (edit_flags & MULTILINE) {
        text_box = new Gwen::Controls::TextBoxMultiline(base);
        base_control = text_box;
        scroller = NULL;
    } else {
        text_box = new Gwen::Controls::TextBox(base);
        scroller = NULL;
        base_control = text_box;
    }

    if (edit_flags & READ_ONLY)
        text_box->SetEditable(false);

    text_box->SetCursor(Gwen::CursorType::Beam);
}

void EditObject::update_text()
{
    new_text = text_box->GetText().Get();
    replace_substring(new_text, unix_newline_character, newline_character);
}
#endif

EditObject::~EditObject()
{
#ifdef CHOWDREN_USE_GWEN
    text_box->DelayedDelete();
    if (scroller)
        scroller->DelayedDelete();
#endif
}


void EditObject::update()
{
#ifdef CHOWDREN_USE_GWEN
    base_control->SetHidden(!get_visible());
    base_control->SetPos(x - frame->off_x, y - frame->off_y);
    base_control->SetSize(width, height);
    if (scroller) {    
        text_box->SizeToContents();
        text_box->SetSize(std::max(text_box->Width(), width),
                          std::max(text_box->Height(), height));
    }
    update_text();
#else
    if (is_mouse_pressed_once(SDL_BUTTON_LEFT)) {
        int mx, my;
        frame->get_mouse_pos(&mx, &my);
        PointCollision col(mx, my);
        if (collide(&col, &edit_col))
            edit_flags |= FOCUS;
        else
            edit_flags &= ~FOCUS;
    }

    if (!(edit_flags & FOCUS))
        return;

    if (limit == -1 || int(text.size()) < limit)
        text += manager.input;

    if (is_key_pressed_once(SDLK_BACKSPACE))
        text = text.substr(0, text.size() - 1);
    if (is_key_pressed_once(SDLK_RETURN))
        edit_flags &= ~FOCUS;
#endif
}

void EditObject::draw()
{
#ifdef CHOWDREN_USE_GWEN
#else
    if (!init_font()) {
        set_visible(false);
        return;
    }

    // black background

    Render::draw_quad(x, y, x + width, y + height, Color(0, 0, 0, 255));

    // content box
    const float off = 1.0f;
    float x1 = x + off;
    float y1 = y + off;
    float x2 = x + width - off;
    float y2 = y + height - off;

    Color color;
    if (edit_flags & FOCUS)
        color.set(230, 230, 230, 255);
    else
        color.set(255, 255, 255, 255);

    Render::draw_quad(x1, y1, x2, y2, color);

    FTTextureFont::color.set(0, 0, 0, 255);
    float xx = x1 + 5.0f;
    int yy = y + font->Ascender() + height * 0.5 - font->LineHeight() * 0.5;
    font->Render(text.c_str(), -1, FTPoint(xx, yy), FTPoint());
#endif
}

void EditObject::set_text(const std::string & value)
{
#ifdef CHOWDREN_USE_GWEN
    text_box->SetText(Gwen::TextObject(value), false);
    update_text();
#else
    text = value;
#endif
}

const std::string & EditObject::get_text()
{
#ifdef CHOWDREN_USE_GWEN
    return new_text;
#else
    return text;
#endif
}

bool EditObject::get_focus()
{
    return (edit_flags & FOCUS) != 0;
}

void EditObject::set_limit(int size)
{
    limit = size;
}

void EditObject::disable_focus()
{
    edit_flags &= ~FOCUS;
}


void EditObject::enable_focus()
{
    edit_flags |= FOCUS;
}


#endif

void EditObject::disable()
{
#ifdef CHOWDREN_USE_GWEN
    text_box->SetDisabled(true);
    text_box->SetEditable(false);
#else
    std::cout << "EditObject::disable not implemented" << std::endl;
#endif
}

void EditObject::scroll_to_end()
{
    std::cout << "EditObject::scroll_to_end not implemented" << std::endl;
}