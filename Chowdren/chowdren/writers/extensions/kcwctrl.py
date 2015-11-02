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

from chowdren.writers.events import (StaticActionWriter, StaticConditionWriter,
    ExpressionMethodWriter, StaticExpressionWriter, make_table,
    ActionMethodWriter, EmptyAction)

class WindowControl(ObjectWriter):
    class_name = 'WindowControl'
    filename = 'windowcontrol'
    use_alterables = True

class WindowWidth(ExpressionMethodWriter):
    has_object = False
    method = '.WINDOW_WIDTH'

class WindowHeight(ExpressionMethodWriter):
    has_object = False
    method = '.WINDOW_HEIGHT'

class WindowName(ExpressionMethodWriter):
    has_object = False
    method = '.NAME'

class SetFrameHeight(ActionMethodWriter):
    has_object = False
    method = 'set_height(%s, %s == 1)'

class SetFrameWidth(ActionMethodWriter):
    has_object = False
    method = 'set_width(%s, %s == 1)'

actions = make_table(StaticActionWriter, {
    0 : 'set_x',
    1 : 'set_y',
    2 : 'set_placement',
    3 : 'set_width',
    4 : 'set_height',
    7 : EmptyAction, # always on top, not portable
    9 : 'set_visible(true)',
    10 : 'set_visible(false)',
    11 : 'minimize',
    17 : 'set_position',
    12 : 'maximize',
    13 : 'restore',
    18 : 'set_title',
    21 : 'set_focus(true)',
    28 : SetFrameWidth,
    29 : SetFrameHeight
})

conditions = make_table(StaticConditionWriter, {
    1 : 'is_maximized',
    4 : 'has_focus'
})

expressions = make_table(StaticExpressionWriter, {
    0 : 'get_x()',
    1 : 'get_y()',
    2 : 'get_width()',
    3 : 'get_height()',
    4 : 'get_screen_width()',
    5 : 'get_screen_height()',
    7 : 'get_width()', # client area
    8 : 'get_height()', # client area
    9 : WindowName,
    11 : 'get_x()',
    12 : 'get_y()',
    13 : WindowWidth,
    14 : WindowHeight
})

def get_object():
    return WindowControl