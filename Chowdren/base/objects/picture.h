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

#ifndef CHOWDREN_PICTURE_H
#define CHOWDREN_PICTURE_H

#include "frameobject.h"
#include <string>
#include "image.h"
#include "collision.h"
#include "types.h"
#include "color.h"

class ActivePicture : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(ActivePicture)

    enum PictureFlags
    {
        HORIZONTAL_FLIP = 1 << 0,
        FORCE_RESIZE = 1 << 1
    };


    Image * image;
    unsigned int picture_flags;
    std::string filename;
    TransparentColor transparent_color;
    float scale_x, scale_y;
    int angle;
    SpriteCollision sprite_col;

#ifdef CHOWDREN_PICTURE_OFFSET
    int offset_y;
#endif

    ActivePicture(int x, int y, int type_id);
    ~ActivePicture();
    void load(const std::string & fn);
    void set_transparent_color(const Color & color);
    void set_hotspot(int x, int y);
    void set_hotspot_mul(float x, float y);
    void set_size(int w, int h);
    void flip_horizontal();
    void set_scale(float value);
    void set_zoom(float value);
    void set_angle(int value, int quality = 0);
    float get_zoom_x();
    int get_width();
    int get_height();
    void draw();
    void paste(int dest_x, int dest_y, int src_x, int src_y,
               int src_width, int src_height, int collision_type);
    int get_resized_width();
    int get_resized_height();
    void set_offset_y(int value);
    void set_wrap(bool enabled);
};

extern FrameObject * default_picture_instance;

#endif // CHOWDREN_PICTURE_H
