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

#ifndef CHOWDREN_QUICKBACKDROP_H
#define CHOWDREN_QUICKBACKDROP_H

#include "frameobject.h"
#include "chowconfig.h"
#include "color.h"

class QuickBackdrop : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(QuickBackdrop)

    Color color;
    int gradient_type;
    Color color2;
    Color outline_color;
    int outline;
    Image * image;

    QuickBackdrop(int x, int y, int type_id);
    ~QuickBackdrop();
    void draw();

#ifdef CHOWDREN_LAYER_WRAP
    int x_offset, y_offset;
    void set_backdrop_offset(int dx, int dy);
#endif
};

#endif // CHOWDREN_QUICKBACKDROP_H
