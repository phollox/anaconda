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

from chowdren.writers.events import (StaticConditionWriter, StaticActionWriter,
                                     StaticExpressionWriter,
                                     ExpressionMethodWriter,
                                     make_table, EmptyAction)

class ClockObject(ObjectWriter):
    class_name = 'ClockObject'
    static = True

    def write_init(self, writer):
        pass

actions = make_table(StaticActionWriter, {
})

conditions = make_table(StaticConditionWriter, {
})

class ClockExpression(ExpressionMethodWriter):
    has_object = False

expressions = make_table(ClockExpression, {
    1 : 'platform_get_datetime().sec',
    2 : 'platform_get_datetime().min',
    3 : 'platform_get_datetime().hour',
    5 : 'platform_get_datetime().mday', # day of month
    6 : 'platform_get_datetime().mon', # month
    7 : 'platform_get_datetime().year' # retrieve year
})

def get_object():
    return ClockObject
