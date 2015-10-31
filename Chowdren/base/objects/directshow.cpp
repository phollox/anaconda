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

#include "objects/directshow.h"
#include "include_gl.h"
#include "collision.h"
#include <iostream>

DirectShow::DirectShow(int x, int y, int type_id)
: FrameObject(x, y, type_id)
{
    collision = new InstanceBox(this);
}

DirectShow::~DirectShow()
{
    delete collision;
}

void DirectShow::load(const std::string & filename)
{
    std::cout << "Load DirectShow file: " << filename << std::endl;
}

void DirectShow::play()
{
    std::cout << "Play DirectShow" << std::endl;
}

void DirectShow::set_width(int value)
{
    width = value;
}

void DirectShow::set_height(int value)
{
    height = value;
}

bool DirectShow::is_playing()
{
    return true;
}

int DirectShow::get_time()
{
    // XXX stupid hack to skip cutscenes
    return 99999;
}

int DirectShow::get_duration()
{
    return 0;
}
