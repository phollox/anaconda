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

#ifndef CHOWDREN_VIEWPORT_H
#define CHOWDREN_VIEWPORT_H

#include "frameobject.h"
#include "render.h"

class Viewport : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(Viewport)

    int center_x, center_y;
    int src_width, src_height;
    Texture texture;
    static Viewport * instance;

    Viewport(int x, int y, int type_id);
    ~Viewport();
    void set_source(int center_x, int center_y, int width, int height);
    void set_width(int w);
    void set_height(int h);
    void draw();
};

#endif // CHOWDREN_VIEWPORT_H
