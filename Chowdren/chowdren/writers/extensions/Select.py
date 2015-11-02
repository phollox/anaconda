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
    ConditionMethodWriter, ExpressionMethodWriter, make_table)
from chowdren.stringhash import get_string_int_map

class SelectObject(ObjectWriter):
    class_name = 'SelectObject'
    static = True

    def write_init(self, writer):
        pass

class LowestValue(ConditionMethodWriter):
    custom = True

    def write(self, writer):
        obj = self.parameters[0].loader
        obj = (obj.objectInfo, obj.objectType)
        writer.start_brace()
        writer.putln('FrameObject * selected; selected = NULL;')
        writer.putlnc('int alt_index = %s;', self.convert_index(1))
        writer.putln('double lowest = 0.0;')
        writer.putln('double test_val;')
        with self.converter.iterate_object(obj, writer, copy=False):
            writer.putlnc('FrameObject * obj = %s;',
                          self.converter.get_object(obj))
            check = 'obj->alterables->values.get(alt_index)'
            writer.putlnc('test_val = %s;', check)
            writer.putlnc('if (selected == NULL || test_val < lowest) {')
            writer.indent()
            writer.putlnc('selected = obj;')
            writer.putlnc('lowest = test_val;')
            writer.end_brace()

        obj_list = self.converter.create_list(obj, writer)

        writer.putlnc('if (selected == NULL) %s', self.converter.event_break)
        writer.putlnc('%s.select_single(selected);', obj_list)
        writer.end_brace()


actions = make_table(ActionMethodWriter, {
})

conditions = make_table(ConditionMethodWriter, {
    2 : LowestValue
})

expressions = make_table(ExpressionMethodWriter, {
})

def get_object():
    return SelectObject
