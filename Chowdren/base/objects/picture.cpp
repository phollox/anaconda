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

#include "objects/picture.h"

// ActivePicture

ActivePicture::ActivePicture(int x, int y, int type_id)
: FrameObject(x, y, type_id), image(NULL), picture_flags(0),
  scale_x(1.0f), scale_y(1.0f), angle(0)
{
    sprite_col.instance = this;
    collision = &sprite_col;
#ifdef CHOWDREN_PICTURE_OFFSET
    offset_y = 0;
#endif
}

ActivePicture::~ActivePicture()
{
    image = NULL;
}

void ActivePicture::load(const std::string & fn)
{
#ifndef NDEBUG
    if (fn != filename)
        std::cout << "load filename: " << fn << std::endl;
#endif
    filename = fn;
    std::string path;
#if defined(CHOWDREN_IS_WIIU) || defined(CHOWDREN_EMULATE_WIIU)
    // small hack to load language-specific files for menu
    size_t dir_end = fn.find_last_of(PATH_SEP);
    size_t dir_start = fn.find_last_of(PATH_SEP, dir_end-1);
    std::string dir = fn.substr(dir_start+1, dir_end-dir_start-1);
    if (dir == "Menu") {
        std::string name = fn.substr(dir_end + 1);
        path = convert_path(fn.substr(0, dir_end+1) +
                            platform_get_language() + "/" + name);

    } else
        path = convert_path(fn);
#else
    path = convert_path(fn);
#endif

    image = get_image_cache(path, 0, 0, 0, 0, transparent_color);

    if (image == NULL)
        return;

    sprite_col.set_image(image, 0, 0);
    if (picture_flags & FORCE_RESIZE)
        set_size(width, height);
}

void ActivePicture::set_transparent_color(const Color & color)
{
    transparent_color = color;
}

void ActivePicture::set_hotspot(int x, int y)
{
    if (image == NULL)
        return;
    SpriteCollision * col = (SpriteCollision*)collision;
    this->x += x - sprite_col.hotspot_x;
    this->y += y - sprite_col.hotspot_y;
    ((SpriteCollision*)collision)->set_hotspot(x, y);
}

void ActivePicture::set_hotspot_mul(float x, float y)
{
    if (image == NULL)
        return;
    set_hotspot(image->width * x, image->height * y);
}

void ActivePicture::flip_horizontal()
{
    picture_flags ^= HORIZONTAL_FLIP;
}

void ActivePicture::set_scale(float value)
{
    ((SpriteCollision*)collision)->set_scale(value);
    scale_x = scale_y = value;
}

void ActivePicture::set_size(int w, int h)
{
	if (image == NULL)
		return;
    float sx = w / float(image->width);
    float sy = h / float(image->height);
    scale_x = sx;
    scale_y = sy;
    ((SpriteCollision*)collision)->set_scale(sx, sy);
}

void ActivePicture::set_zoom(float value)
{
    set_scale(value / 100.0);
}

void ActivePicture::set_angle(int value, int quality)
{
    ((SpriteCollision*)collision)->set_angle(value);
    angle = value;
}

float ActivePicture::get_zoom_x()
{
    return scale_x * 100.0f;
}

int ActivePicture::get_width()
{
    if (image == NULL)
        return 0;
    return image->width;
}

int ActivePicture::get_height()
{
    if (image == NULL)
        return 0;
    return image->height;
}

int ActivePicture::get_resized_width()
{
    if (image == NULL)
        return 0;
    return ((SpriteCollision*)collision)->width;
}

int ActivePicture::get_resized_height()
{
    if (image == NULL)
        return 0;
    return ((SpriteCollision*)collision)->height;
}

void ActivePicture::draw()
{
    if (image == NULL)
        return;
    image->hotspot_x = sprite_col.hotspot_x;
    image->hotspot_y = sprite_col.hotspot_y;
#ifdef CHOWDREN_PICTURE_OFFSET
    if (offset_y != 0) {
        image->upload_texture();
        int ww = image->width * scale_x;
        int hh = image->height * scale_y;

        int x2 = x + ww;
        int y2 = y + hh - offset_y;

        float t_x1 = 0.0f;
        float t_x2 = 1.0f;
        float t_y1 = float(offset_y) / float(hh);
        float t_y2 = 1.0f;
        Render::draw_tex(x, y, x2, y2, blend_color, image->tex,
                         t_x1, t_y1, t_x2, t_y2);
        return;
    }
#endif
    draw_image(image, x, y, blend_color, angle, scale_x, scale_y,
               picture_flags & HORIZONTAL_FLIP);
}

void ActivePicture::paste(int dest_x, int dest_y, int src_x, int src_y,
                          int src_width, int src_height, int collision_type)
{
    if (image == NULL) {
        std::cout << "Invalid image paste: " << filename << std::endl;
        return;
    }
    image->hotspot_x = 0;
    image->hotspot_y = 0;
    layer->paste(image, dest_x, dest_y, src_x, src_y,
                 src_width, src_height, collision_type, effect, blend_color);
}

void ActivePicture::set_offset_y(int value)
{
#ifdef CHOWDREN_PICTURE_OFFSET
    offset_y = value;
    sprite_col.flags |= BOX_COLLISION;
    sprite_col.type = SPRITE_BOX;
#endif
}

void ActivePicture::set_wrap(bool value)
{
    std::cout << "Set wrap not implementd: " << value << std::endl;
}

class DefaultPicture : public ActivePicture
{
public:
    DefaultPicture()
    : ActivePicture(0, 0, 0)
    {
        collision = new InstanceBox(this);
        create_alterables();
        setup_default_instance(this);
    }
};

static DefaultPicture default_picture;
FrameObject * default_picture_instance = &default_picture;
