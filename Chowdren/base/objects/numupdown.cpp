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

#include "numupdown.h"
#include "gui/gwen.h"
#include "collision.h"
#include "manager.h"

// NumericUpDown

NumericUpDown::NumericUpDown(int x, int y, int type_id)
: FrameObject(x, y, type_id)
#ifdef CHOWDREN_USE_GWEN
, control(manager.frame->gwen.frame_base)
#endif
{
#ifdef CHOWDREN_USE_GWEN
    collision = new InstanceBox(this);
#endif
}

NumericUpDown::~NumericUpDown()
{
#ifdef CHOWDREN_USE_GWEN
    delete collision;
#endif
}

void NumericUpDown::init_control(int value, int min, int max)
{
#ifdef CHOWDREN_USE_GWEN
    control.SetMin(min);
    control.SetMax(max);
    control.SetValue(value);
    control.SetPos(x, y);
    control.SetSize(width, height);
#endif
}

void NumericUpDown::update()
{
#ifdef CHOWDREN_USE_GWEN
    control.SetHidden(!get_visible());
    control.SetPos(x, y);
    control.SetSize(width, height);
#endif
}

void NumericUpDown::draw()
{
#ifdef CHOWDREN_USE_GWEN
    control.SetHidden(!get_visible());
    control.SetPos(x, y);
    control.SetSize(width, height);
#endif
}

void NumericUpDown::set_value(int value)
{
#ifdef CHOWDREN_USE_GWEN
    control.SetValue(value);
#endif
}

int NumericUpDown::get_value()
{
#ifdef CHOWDREN_USE_GWEN
    return control.GetIntValue();
#endif
}