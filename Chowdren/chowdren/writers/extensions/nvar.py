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
from mmfparser.bitdict import BitDict

from chowdren.common import get_animation_name, to_c, make_color

from chowdren.writers.events import (ComparisonWriter, ActionMethodWriter,
    ConditionMethodWriter, ExpressionMethodWriter, make_table)

from chowdren.writers.extensions.AssArray import (AssociateArray,   
                                                  create_key_action,
                                                  create_key_expression)

class NamedVariables(AssociateArray):
    use_alterables = True

    def write_init(self, writer):
        data = self.get_data()
        data.skipBytes(4)
        data.skipBytes(4) # width, height
        is_global = data.readInt() != 0
        case_insensitive = data.readInt() != 0
        if case_insensitive:
            raise NotImplementedError()
        self.write_assarray(writer, is_global)

actions = make_table(ActionMethodWriter, {
    0 : create_key_action('set_value'),
    1 : create_key_action('add_value'),
    2 : create_key_action('sub_value'),
    8 : create_key_action('set_string'),
    11 : 'clear'
})

conditions = make_table(ConditionMethodWriter, {
})

expressions = make_table(ExpressionMethodWriter, {
    0 : create_key_expression('get_value'),
    1 : create_key_expression('get_string')
})

def get_object():
    return NamedVariables
