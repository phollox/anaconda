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

from chowdren.writers.events import (ConditionMethodWriter,
    ActionMethodWriter, ExpressionMethodWriter, make_table, EmptyAction,
    ComparisonWriter, make_comparison)

class DirectShow(ObjectWriter):
    class_name = 'DirectShow'
    filename = 'directshow'
    use_alterables = True

    def write_init(self, writer):
        pass

actions = make_table(ActionMethodWriter, {
    0 : 'load',
    2 : 'play',
    4 : 'set_width',
    5 : 'set_height',
    20 : 'set_visible(true)'
})

conditions = make_table(ConditionMethodWriter, {
    0 : 'is_playing',
    2 : make_comparison('get_time()'),
})

expressions = make_table(ExpressionMethodWriter, {
    2 : 'get_duration()',
    3 : 'get_time()'
})

def get_object():
    return DirectShow
