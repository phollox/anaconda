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

#ifndef CHOWDREN_WINDOWCONTROL_H
#define CHOWDREN_WINDOWCONTROL_H

#include "frameobject.h"
#include <string>

class WindowControl : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(WindowControl)

    WindowControl(int x, int y, int type_id);

    static int get_x();
    static int get_y();
    static void set_x(int x);
    static void set_y(int y);
    static void set_position(int x, int y);
    static bool has_focus();
    static bool is_maximized();
    static void set_focus(bool value);
    static void set_width(int w);
    static void set_height(int w);
    static void maximize();
    static void restore();
    static int get_width();
    static int get_height();
    static int get_screen_width();
    static int get_screen_height();
    static void set_visible(bool value);
    static void minimize();
    static void set_placement(int value);
    static void set_title(const std::string & title);
    static void set_frame_width(int width, bool adjust);
    static void set_frame_height(int height, bool adjust);
};

#endif // CHOWDREN_WINDOWCONTROL_H
