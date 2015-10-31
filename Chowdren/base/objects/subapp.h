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

#ifndef CHOWDREN_SUBAPP_H
#define CHOWDREN_SUBAPP_H

#include "frameobject.h"
#include "events.h"

#ifdef CHOWDREN_USE_GWEN
#include "Gwen/Controls/WindowControl.h"
#endif

class SubApplication : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(SubApplication)

    static SubApplication * current;
    static int current_x, current_y;
    Frames subapp_frame;
    int frame_offset;
    bool done;
    bool starting;
    bool old_ignore_controls;

#ifdef CHOWDREN_USE_GWEN
    enum Flags {
        IS_POPUP = 1 << 0,
        IS_DOCKED = 1 << 1
    };

    bool gwen_close;
    int subapp_flags;
    int start_x, start_y;
    Gwen::Controls::WindowControl * window_control;
    int get_render_x();
    int get_render_y();
    void init_window();
    void init_frame();
#endif

    SubApplication(int x, int y, int id);
    ~SubApplication();
    void restart(int index);
    void update();
    void set_next_frame(int index);
    void set_frame(int index);
#ifdef CHOWDREN_SUBAPP_FRAMES
    void draw_subapp();
    static void draw_frames();
    static bool test_pos(Frame * frame);
#endif
};

#endif // CHOWDREN_SUBAPP_H
