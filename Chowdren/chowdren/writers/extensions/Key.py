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
    ConditionMethodWriter, ExpressionMethodWriter, EmptyAction)

class KeyObject(ObjectWriter):
    class_name = 'Keyboard'
    static = True

    def write_init(self, writer):
        pass

class AlwaysCondition(ConditionMethodWriter):
    is_always = True

actions = make_table(StaticActionWriter, {
    1 : EmptyAction,
    0 : EmptyAction
})

conditions = make_table(AlwaysCondition, {
    1 : 'is_key_pressed(translate_string_to_key(%s))',
    # 3 : 'is_key_pressed_once(translate_vk_to_key(%s))',
    # 6 : 'is_key_released_once(translate_vk_to_key(%s))',
    3 : 'is_key_pressed_once(%s)',
    6 : 'is_key_released_once(%s)',
    7 : 'is_key_released_once(translate_string_to_key(%s))'
})

expressions = make_table(ExpressionMethodWriter, {
})

def get_object():
    return KeyObject