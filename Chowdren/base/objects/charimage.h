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

#ifndef CHOWDREN_CHARIMAGE_H
#define CHOWDREN_CHARIMAGE_H

#include "frameobject.h"
#include <string>
#include "types.h"
#include "font.h"

struct CharacterImageAttributes
{
    int values[3];

    CharacterImageAttributes()
    : values()
    {
    }
};

struct CharacterImageLine
{
    int width, height;
    int x_align, y_align;

    CharacterImageLine()
    : x_align(ALIGN_LEFT), y_align(ALIGN_TOP), width(0), height(0)
    {
    }
};

struct CharacterImageBlock
{
    int line;
    CharacterImageAttributes attribs;
    std::string text;

    CharacterImageBlock(int line, const CharacterImageAttributes & attribs,
                        const std::string & text)
    : line(line), attribs(attribs), text(text)
    {
    }
};

struct CharacterImage
{
    Image * image;
    int width, height;
};

struct CharacterImageAlias
{
    CharacterImage charmap[256];
};

class CharacterImageObject : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(CharacterImageObject)

    CharacterImageAlias * aliases;
    std::string text;
    std::string unformatted;
    int x_off;

    int total_height[3];

    vector<CharacterImageLine> lines;
    vector<CharacterImageBlock> blocks;

    CharacterImageObject(int x, int y, int type_id);
    ~CharacterImageObject();
    void set_text(const std::string & text);
    void update_text();
    void draw();
    std::string get_char(int index);
    int get_char_width(int alias, const std::string & c);
    void set_char_width(int alias, const std::string & c, int width);
    void set_clipping_width(int alias, const std::string & c, int width);
    void load(int alias, const std::string & c, const std::string & path,
              int x_hotspot, int y_hotspot);
};

#endif // CHOWDREN_CHARIMAGE_H
