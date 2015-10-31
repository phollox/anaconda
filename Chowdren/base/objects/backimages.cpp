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

#include "objects/backimages.h"
#include "collision.h"

BackImages::BackImages(int x, int y, int type_id)
: FrameObject(x, y, type_id), x_off(0), y_off(0)
{
}

int BackImages::get_width(int slot)
{
    return width;
}

class DefaultInstance : public BackImages
{
public:
    DefaultInstance()
    : BackImages(0, 0, 0)
    {
        collision = new InstanceBox(this);
        create_alterables();
        setup_default_instance(this);
    }
};

static DefaultInstance default_backimages;
FrameObject * default_backimages_instance = &default_backimages;
