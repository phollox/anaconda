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
from mmfparser.bitdict import BitDict

class NumericUpDown(ObjectWriter):
    class_name = 'NumericUpDown'
    filename = 'numupdown'

    def write_init(self, writer):
        if not self.has_updates():
            return
        data = self.get_data()
        width = data.readInt()
        height = data.readInt()
        writer.putlnc('width = %s;', width)
        writer.putlnc('height = %s;', height)
        data.skipBytes(88)
        value = data.readInt()
        minimum = data.readInt()
        maximum = data.readInt()
        writer.putlnc('init_control(%s, %s, %s);', value, minimum, maximum)

    def has_updates(self):
        return self.converter.config.use_gwen()

actions = make_table(ActionMethodWriter, {
    0 : 'set_value',
    14 : 'set_visible(false)',
    15 : 'set_visible(true)'
})

conditions = make_table(ConditionMethodWriter, {
})

expressions = make_table(ExpressionMethodWriter, {
    0 : 'get_value()'
})

def get_object():
    return NumericUpDown
