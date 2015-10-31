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

#ifndef CHOWDREN_BACKDROP_H
#define CHOWDREN_BACKDROP_H

#include "frameobject.h"
#include "chowconfig.h"

class Backdrop : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(Backdrop)

    Image * image;
#if defined(CHOWDREN_IS_WIIU) || defined(CHOWDREN_EMULATE_WIIU)
    int remote;
#endif

    Backdrop(int x, int y, int type_id);
    ~Backdrop();
    void draw();

#ifdef CHOWDREN_USE_BACKMAGIC
    int orig_x, orig_y;

    int get_orig_x()
    {
        return orig_x + layer->off_x;
    }

    int get_orig_y()
    {
        return orig_y + layer->off_y;
    }
#endif

};

#endif // CHOWDREN_BACKDROP_H
