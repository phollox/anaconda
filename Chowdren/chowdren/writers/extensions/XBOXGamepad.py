# Copyright (c) Mathias Kaerlev 2012-2015.
#
# This file is part of Anaconda.
#
# Anaconda is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Anaconda is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Anaconda.  If not, see <http://www.gnu.org/licenses/>.

from chowdren.writers.objects import ObjectWriter

from chowdren.common import get_animation_name, to_c, make_color

from chowdren.writers.events import (StaticConditionWriter,
    StaticActionWriter, StaticExpressionWriter, make_table,
    ConditionMethodWriter, ExpressionMethodWriter, EmptyAction,
    StaticConditionWriter, ActionMethodWriter)

class XBOXGamepad(ObjectWriter):
    class_name = 'Joystick'
    static = True

actions = make_table(ActionMethodWriter, {
    0 : 'joystick_vibrate'
})

conditions = make_table(ConditionMethodWriter, {
    0 : 'is_joystick_attached(%s+1)',
    2 : 'is_joystick_pressed(%s, CHOWDREN_BUTTON_B)',
    1 : 'is_joystick_pressed(%s, CHOWDREN_BUTTON_A)',
    9 : 'is_joystick_pressed(%s, CHOWDREN_BUTTON_RIGHTSHOULDER)',
    3 : 'is_joystick_pressed(%s, CHOWDREN_BUTTON_X)',
    4 : 'is_joystick_pressed(%s, CHOWDREN_BUTTON_Y)',
    5 : 'is_joystick_pressed(%s, CHOWDREN_BUTTON_BACK)',
    7 : 'is_joystick_pressed(%s, CHOWDREN_BUTTON_LEFTSHOULDER)',
    14 : 'is_joystick_pressed(%s, CHOWDREN_BUTTON_DPAD_LEFT)',
    15 : 'is_joystick_pressed(%s, CHOWDREN_BUTTON_DPAD_RIGHT)',
    12 : 'is_joystick_pressed(%s, CHOWDREN_BUTTON_DPAD_UP)',
    11 : 'is_joystick_pressed(%s, CHOWDREN_BUTTON_START)',
    13 : 'is_joystick_pressed(%s, CHOWDREN_BUTTON_DPAD_DOWN)'
})

expressions = make_table(ExpressionMethodWriter, {
    4 : 'get_joystick_lt',
    5 : 'get_joystick_rt'
})

def get_object():
    return XBOXGamepad