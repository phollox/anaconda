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
    ConditionMethodWriter, ExpressionMethodWriter)

class Keyboard(ObjectWriter):
    class_name = 'Keyboard'
    static = True

    def write_init(self, writer):
        pass

class OnAnyDown(ConditionMethodWriter):
    is_always = True

    def write(self, writer):
        writer.put('last_key != -1')

actions = make_table(StaticActionWriter, {
})

conditions = make_table(ConditionMethodWriter, {
    3 : OnAnyDown,
    2 : 'is_key_pressed',
    0 : 'is_key_pressed_once',
    5 : 'is_any_key_pressed'
})

expressions = make_table(ExpressionMethodWriter, {
    0 : '.last_key',
    2 : 'translate_key_to_string'
})

def get_object():
    return Keyboard