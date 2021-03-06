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

#include "objects/viewport.h"
#include "chowconfig.h"
#include "collision.h"
#include "render.h"

// Viewport

Viewport * Viewport::instance = NULL;

Viewport::Viewport(int x, int y, int type_id)
: FrameObject(x, y, type_id)
{
    collision = new InstanceBox(this);
    instance = this;
}

Viewport::~Viewport()
{
    delete collision;
    instance = NULL;
}

void Viewport::set_source(int center_x, int center_y, int width, int height)
{
    this->center_x = center_x;
    this->center_y = center_y;
    src_width = width;
    src_height = height;
}

void Viewport::set_width(int w)
{
    width = w;
    collision->update_aabb();
}

void Viewport::set_height(int h)
{
    height = h;
    collision->update_aabb();
}

void Viewport::draw()
{
    if (src_width == width && src_height == height)
        return;
    if (src_width == 0 || src_height == 0)
        return;
    int src_x1 = center_x - src_width / 2;
    int src_y1 = center_y - src_height / 2;
    int src_x2 = src_x1 + src_width;
    int src_y2 = src_y1 + src_height;
    Texture t = Render::copy_rect(src_x1, src_y1, src_x2, src_y2);
    int x2 = x + width;
    int y2 = y + height;
    Render::disable_blend();
    Render::draw_tex(x, y, x2, y2, Color(255, 255, 255, 255), t,
                     back_texcoords[0], back_texcoords[1],
                     back_texcoords[2], back_texcoords[3]);
    Render::enable_blend();
}
