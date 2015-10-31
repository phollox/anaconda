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

#ifndef CHOWDREN_BUTTONEXT_H
#define CHOWDREN_BUTTONEXT_H

#include "frameobject.h"
#include <string>

#ifdef CHOWDREN_USE_GWEN
#include "Gwen/Controls/Button.h"
#endif

class ButtonObject : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(ButtonObject)

    enum {
        IS_CHECKBOX = 1 << 0
    };

#ifdef CHOWDREN_USE_GWEN
    unsigned int button_flags;
    Gwen::Controls::Button * button;
    int clicked;
#endif

    ButtonObject(int x, int y, int type_id);
    ~ButtonObject();
    void init_button(unsigned int flags);
    void update();
    void draw();
    void check();
    void uncheck();
    void set_text(const std::string & text);
    void enable();
    void disable();
    bool is_clicked();
};

#endif // CHOWDREN_BUTTONEXT_H
