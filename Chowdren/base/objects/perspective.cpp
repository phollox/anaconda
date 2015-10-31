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

#include "objects/perspective.h"
#include "include_gl.h"
#include "collision.h"
#include <iostream>
#include "render.h"

PerspectiveObject::PerspectiveObject(int x, int y, int type_id)
: FrameObject(x, y, type_id)
{
    collision = new InstanceBox(this);

    set_shader(Render::PERSPECTIVE);
}

PerspectiveObject::~PerspectiveObject()
{
    delete collision;
}

void PerspectiveObject::set_width(int width)
{
    this->width = width;
    collision->update_aabb();
}

void PerspectiveObject::draw()
{
    int box[4];
    get_screen_aabb(box);
    Texture t = Render::copy_rect(box[0], box[1], box[2], box[3]);
    begin_draw();
    Render::disable_blend();
    Render::draw_tex(x, y, x + width, y + height, Color(255, 255, 255, 255),
                     t,
                     back_texcoords[0], back_texcoords[1],
                     back_texcoords[2], back_texcoords[3]);
    Render::enable_blend();
    end_draw();
}

void PerspectiveObject::set_waves(double value)
{
    set_shader_parameter("sine_waves", value);
}

void PerspectiveObject::set_zoom(double value)
{
    set_shader_parameter("zoom", std::max(0.0, value));
}

void PerspectiveObject::set_offset(double value)
{
    set_shader_parameter("offset", value);
}
