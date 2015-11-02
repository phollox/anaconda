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
from chowdren.writers.events import (ComparisonWriter, StaticActionWriter,
    StaticConditionWriter, StaticExpressionWriter, make_table,
    ActionMethodWriter, ExpressionMethodWriter)

class StringReplace(ObjectWriter):
    class_name = 'StringReplace'
    filename = 'stringreplace'

    def write_init(self, writer):
        data = self.get_data()
        case_sensitive = data.readByte() == 1
        use_match_count = data.readByte() == 1
        data.skipBytes(2) # padding
        match_count = data.readInt()
        right_to_left = data.readByte() == 1

        if case_sensitive:
            raise NotImplementedError()
        if use_match_count:
            raise NotImplementedError()
        if right_to_left:
            raise NotImplementedError()

class ReplaceExpression(StaticExpressionWriter):
    method = 'replace'

actions = make_table(ActionMethodWriter, {
    1 : 'add_replacement'
})

conditions = make_table(StaticConditionWriter, {
})

expressions = make_table(ExpressionMethodWriter, {
    0 : 'replace',
    1 : ReplaceExpression
})

def get_object():
    return StringReplace