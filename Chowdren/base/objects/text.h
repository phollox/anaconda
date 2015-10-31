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

#ifndef CHOWDREN_TEXT_H
#define CHOWDREN_TEXT_H

#include "frameobject.h"
#include <string>
#include "types.h"
#include "font.h"

class FTSimpleLayout;

class Text : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(Text)

    vector<std::string> paragraphs;
    std::string text;
    unsigned int current_paragraph;
    bool initialized;
    int alignment;
    bool bold, italic;
    std::string font_name;
    FTTextureFont * font;
    std::string draw_text;
    bool draw_text_set;
    FTSimpleLayout * layout;
    float scale;

    Text(int x, int y, int type_id);
    ~Text();
    void add_line(const std::string & text);
    void draw();
    void set_string(const std::string & value);
    void set_paragraph(unsigned int index);
    void next_paragraph();
    int get_index();
    int get_count();
    bool get_bold();
    bool get_italic();
    void set_bold(bool value);
    const std::string & get_paragraph(int index);
    void set_scale(float scale);
    void set_width(int w);
    int get_width();
    int get_height();
    void update_draw_text();
    const std::string & get_font_name();
};

class FontInfo
{
public:
    static std::string vertical_tab;

    static int get_width(FrameObject * obj);
    static int get_height(FrameObject * obj);
    static void set_width(FrameObject * obj, int w);
    static void set_scale(FrameObject * obj, float scale);
};

extern FrameObject * default_text_instance;

#endif // CHOWDREN_TEXT_H
