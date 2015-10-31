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

#ifndef CHOWDREN_ALPHAIMAGE_H
#define CHOWDREN_ALPHAIMAGE_H

#include "frameobject.h"

/*
This object is actually never rendered, but Heart Forth, Alicia depends on
the storage of some values.
*/

struct AlphaImage
{
    int width, height;

    AlphaImage(int w, int h)
    : width(w), height(h)
    {
    }
};

class AlphaImageObject : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(AlphaImageObject)

    int image_count;
    AlphaImage * images;

    int new_width, new_height;
    double scale_x, scale_y;

    int alpha_flags;

    AlphaImage * image;
    int index;
    int anim_frame;
    int hotspot_x, hotspot_y;
    double angle;

    AlphaImageObject(int x, int y, int type_id);
    void set_image(int index);
    void set_hotspot(int x, int y);
    void set_scale_x(double scale);
    void set_scale_y(double scale);
    void set_width(int value);
    void set_height(int value);
    int get_width();
    int get_height();
};

#endif // CHOWDREN_ALPHAIMAGE_H
