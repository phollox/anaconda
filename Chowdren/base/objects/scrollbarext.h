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

#ifndef CHOWDREN_SCROLLBAREXT_H
#define CHOWDREN_SCROLLBAREXT_H

#include "frameobject.h"
#include "common.h"

#include "Gwen/Gwen.h"
#include "Gwen/Controls/ScrollBar.h"
#include "gui/gwen.h"

class ScrollbarObject : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(ScrollbarObject)

    Gwen::Controls::BaseScrollBar* scrollbar;

    bool vertical;
    int min_val;
    int max_val;

    ScrollbarObject(int x, int y, int type_id);
    ~ScrollbarObject();

    void set_width(int width);
    void set_height(int height);
    void set_scroll_range(int min, int max);
    int get_value();

    void init_scrollbar(int value);

    void update();
    void draw();
};

#endif // CHOWDREN_SCROLLBAREXT_H
