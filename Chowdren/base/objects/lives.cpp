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

#include "objects/lives.h"
#include "collision.h"
#include "manager.h"

// Lives

Lives::Lives(int x, int y, int type_id)
: FrameObject(x, y, type_id), flash_interval(0.0f)
{
    collision = new InstanceBox(this);
}

Lives::~Lives()
{
    delete collision;
}

void Lives::flash(float value)
{
    flash_interval = value;
    flash_time = 0.0f;
}

void Lives::update()
{
    update_flash(flash_interval, flash_time);
}

void Lives::draw()
{
    int xx = x;
    int i = 0;
    while (i < manager.lives) {
        image->draw(xx, y, blend_color);
        xx += image->width;
        i++;
    }
}
