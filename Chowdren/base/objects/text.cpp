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

#include "objects/text.h"
#include "collision.h"
#include "common.h"
#include "utfconv.h"

// Text

Text::Text(int x, int y, int type_id)
: FrameObject(x, y, type_id), initialized(false), current_paragraph(0),
  draw_text_set(false), layout(NULL), scale(1.0f)
{
    collision = new InstanceBox(this);
}

Text::~Text()
{
    delete collision;
    delete layout;
}

void Text::add_line(const std::string & text)
{
    paragraphs.push_back(text);
    if (initialized)
        return;
    initialized = true;
    this->text = text;

#ifdef CHOWDREN_FORCE_TEXT_LAYOUT
    if (layout != NULL)
        return;

    set_width(width);
    layout->SetAlignment((TextAlignment)alignment);
#endif
}

#define LAYOUT_USE_ALIGNMENT

void Text::draw()
{
#ifdef CHOWDREN_IS_PS4
    // XXX hack
    return;
#endif
    if (!init_font()) {
        set_visible(false);
        return;
    }

    update_draw_text();

    if (effect == Render::PIXELOUTLINE) {
        Render::set_effect(Render::FONTOUTLINE, this,
                           font->textureWidth, font->textureHeight);
        FTTextureFont::custom_shader = true;
    }

    double off_y = y + font->Ascender();

    FTTextureFont::color = blend_color;
    if (layout != NULL) {
        int lines = layout->get_lines(draw_text.c_str(), -1);
        double box_h = lines * font->LineHeight();
        if (alignment & ALIGN_VCENTER) {
            off_y += (height - box_h) * 0.5;
        } else if (alignment & ALIGN_BOTTOM) {
            off_y += box_h;
        }
        int off_yy = int(off_y);
        layout->Render(draw_text.c_str(), -1, FTPoint(x, off_yy));
    } else {
        if (alignment & ALIGN_VCENTER) {
            off_y += height * 0.5 - font->LineHeight() * 0.5;
        } else if (alignment & ALIGN_BOTTOM) {
            off_y += font->LineHeight();
        }

        FTBBox box = font->BBox(draw_text.c_str(), -1, FTPoint());
        double box_w = box.Upper().X() - box.Lower().X();
        // double box_h = box.Upper().Y() - box.Lower().Y();
        double off_x = x;

        if (alignment & ALIGN_HCENTER)
            off_x += 0.5 * (width - box_w);
        else if (alignment & ALIGN_RIGHT)
            off_x += width - box_w;

#ifdef CHOWDREN_BIG_FONT_OFFY
        if (font == big_font)
            off_y += CHOWDREN_BIG_FONT_OFFY;
#endif
        font->Render(draw_text.c_str(), -1, FTPoint(int(off_x), int(off_y)),
                     FTPoint());
    }

    if (effect == Render::PIXELOUTLINE) {
        Render::disable_effect();
        FTTextureFont::custom_shader = false;
    }
}

void Text::set_string(const std::string & value)
{
    if (text == value)
        return;
    text = value;
    draw_text_set = false;
}

void Text::set_paragraph(unsigned int index)
{
    current_paragraph = index;
    set_string(get_paragraph(index));
}

void Text::next_paragraph()
{
    set_paragraph(current_paragraph + 1);
}

int Text::get_index()
{
    return current_paragraph;
}

int Text::get_count()
{
    return paragraphs.size();
}

bool Text::get_bold()
{
    return bold;
}

bool Text::get_italic()
{
    return italic;
}

void Text::set_bold(bool value)
{
    bold = value;
}

const std::string & Text::get_paragraph(int index)
{
    if (index < 0)
        index = 0;
    else if (index >= (int)paragraphs.size())
        index = paragraphs.size() - 1;
    return paragraphs[index];
}

void Text::update_draw_text()
{
    if (draw_text_set)
        return;
    // convert from windows-1252 to utf-8
    draw_text_set = true;
#ifdef CHOWDREN_TEXT_USE_UTF8
    draw_text = text;
#else
    convert_windows1252_to_utf8(text, draw_text);
#endif
    if (layout != NULL)
        return;
    if (draw_text.find('\n') == std::string::npos)
        return;
    layout = new FTSimpleLayout;
    layout->SetFont(font);
    layout->SetLineLength(width);
}

void Text::set_width(int w)
{
    // XXX should have aabb update
    width = w;
    if (layout == NULL) {
        layout = new FTSimpleLayout;
        layout->SetFont(font);
    }
    layout->SetLineLength(w);
}

void Text::set_scale(float scale)
{
    // XXX should have aabb update
    this->scale = scale;
}

int Text::get_width()
{
    if (layout == NULL)
        return width;
    update_draw_text();
    FTBBox bb = layout->BBox(draw_text.c_str(), text.size());
    return (int)(bb.Upper().X() - bb.Lower().X());
}

int Text::get_height()
{
    if (layout == NULL)
        return height;
    update_draw_text();
    FTBBox bb = layout->BBox(draw_text.c_str(), text.size());
    return (int)(bb.Upper().Y() - bb.Lower().Y());
}

const std::string & Text::get_font_name()
{
    return font_name;
}

// FontInfo

int FontInfo::get_width(FrameObject * obj)
{
    return ((Text*)obj)->get_width();
}

int FontInfo::get_height(FrameObject * obj)
{
    return ((Text*)obj)->get_height();
}

void FontInfo::set_width(FrameObject * obj, int width)
{
    ((Text*)obj)->set_width(width);
}

void FontInfo::set_scale(FrameObject * obj, float scale)
{
    ((Text*)obj)->set_scale(scale);
}


std::string FontInfo::vertical_tab("\x0B");

class DefaultText : public Text
{
public:
    DefaultText()
    : Text(0, 0, 0)
    {
        collision = new InstanceBox(this);
        setup_default_instance(this);
    }
};

static DefaultText default_text;
FrameObject * default_text_instance = &default_text;