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

actions = make_table(StaticActionWriter, {
    0 : 'set_mouse_x',
    1 : 'set_mouse_y'
})

conditions = make_table(StaticConditionWriter, {
})

expressions = make_table(StaticExpressionWriter, {
    0 : 'get_mouse_x()',
    1 : 'get_mouse_y()'
})

def get_object():
    return WindowControl