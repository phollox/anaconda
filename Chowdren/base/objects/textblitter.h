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

#ifndef CHOWDREN_TEXTBLITTER_H
#define CHOWDREN_TEXTBLITTER_H

#include "frameobject.h"
#include "types.h"
#include "chowstring.h"
#include "color.h"
#include "image.h"

enum BlitterAnimation
{
    BLITTER_ANIMATION_NONE = 0,
    BLITTER_ANIMATION_SINWAVE = 1
};

struct LineReference
{
    const char * start;
    int size;

    LineReference(const char * start, int size)
    : start(start), size(size)
    {
    }
};

class TextBlitter : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(TextBlitter)

    vector<LineReference> lines;
    chowstring text;
    int char_width, char_height;
    int char_offset;
    Image * image;
    const chowstring * charmap_str;
    int * charmap;
    float flash_time, flash_interval;
    int alignment;
    int x_spacing, y_spacing;
    int x_scroll, y_scroll;
    int x_off, y_off;
    bool charmap_ref;
    bool wrap;
    bool transparent;
    int image_width;

    int anim_type;
    int anim_speed;
    int anim_frame;
    int wave_freq, wave_height;

    TransparentColor transparent_color;

    enum CallbackFlags
    {
        CHAR_CALLBACK = 1 << 0,
        LINE_CALLBACK = 1 << 1,
        BEGIN_CALLBACK = 1 << 2
    };

    int callback_flags;
    int callback_line_count;
    int callback_line;
    int callback_char; // index
    int callback_transparency;
#ifdef CHOWDREN_BLITTER_EXT_CALLBACKS
    int callback_character_char; // value
    int callback_char_width;
    int callback_char_dst_x;
    int callback_char_src_y;
#endif

    Image * draw_image;
    ReplacedImages replacer;

    TextBlitter(int x, int y, int type_id);
    ~TextBlitter();
    void initialize(const chowstring & charmap);
    void load(const chowstring & filename);
    void set_text(const chowstring & text);
    void append_text(const chowstring & text);
    void update_lines();
    void set_x_spacing(int spacing);
    void set_y_spacing(int spacing);
    void set_x_scroll(int value);
    void set_y_scroll(int value);
    int get_x_align();
    int get_y_align();
    void set_x_align(int value);
    void set_y_align(int value);
    void set_width(int width);
    void set_height(int height);
    void draw();
    void update();
    void flash(float value);
    chowstring get_line(int index);
    int get_line_count();
    chowstring get_map_char(int index);
    void replace_color(Color from, Color to);
    void set_transparent_color(int color);
    void set_animation_parameter(int index, int value);
    void set_animation_type(int value);
    void set_animation_speed(int value);
    void set_charmap(const chowstring & charmap);
    const chowstring & get_charmap();

    const chowstring & get_text()
    {
        return text;
    }

    void set_char_width(int width)
    {
        char_width = std::min(image_width, width);
    }

    virtual void call_char_callback();
    virtual void call_line_callback();
    virtual void call_begin_callback();
};

extern FrameObject * default_blitter_instance;

#endif // CHOWDREN_TEXTBLITTER_H
