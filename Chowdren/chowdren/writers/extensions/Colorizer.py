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
from chowdren.writers.events import (ComparisonWriter, ActionMethodWriter,
    ConditionMethodWriter, ExpressionMethodWriter, make_table,
    EmptyAction)
from mmfparser.data.font import LogFont


class ColorizerObject(ObjectWriter):
    class_name = 'ColorizerObject'
    filename = 'colorizer'
    use_alterables = True

    def write_init(self, writer):
        pass

actions = make_table(ActionMethodWriter, {
    29 : EmptyAction, # load mask from overlay/surface
    32 : EmptyAction, # change mask mode
    0 : 'set_red', # set red out of old red
    4 : 'set_green', # set green out of old green,
    8 : 'set_blue' # set blue out of old blue
})

conditions = make_table(ConditionMethodWriter, {
})

expressions = make_table(ExpressionMethodWriter, {
    2 : '.r', # red out of red,
    6 : '.g', # green out of old green
    10 : '.b' # blue out of old blue
})

def get_object():
    return ColorizerObject
