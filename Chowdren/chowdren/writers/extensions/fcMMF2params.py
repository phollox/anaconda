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
                                     StaticExpressionWriter, ExpressionWriter,
                                     make_table, EmptyAction)

class MMF2Params(ObjectWriter):
    class_name = 'MMFParams'
    static = True

    def write_init(self, writer):
        pass

class GetFrameName(ExpressionWriter):
    def get_string(self):
        return 'data->name'

class GetAboutText(ExpressionWriter):
    def get_string(self):
        return 'ABOUT'

actions = make_table(StaticActionWriter, {
    8 : EmptyAction, # set frame width
})

conditions = make_table(StaticConditionWriter, {
})

expressions = make_table(StaticExpressionWriter, {
    2 : GetFrameName,
    1 : GetAboutText
})

def get_object():
    return MMF2Params
