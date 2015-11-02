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
    StaticActionWriter, StaticExpressionWriter, make_table, ExpressionWriter)

import base64

class FontInfoObject(ObjectWriter):
    class_name = 'FontInfo'
    static = True

    def write_init(self, writer):
        pass

class Base64Decode(ExpressionWriter):
    def get_string(self):
        converter = self.converter
        items = converter.expression_items
        next_exp = items[converter.item_index + 1]
        value = base64.b64decode(next_exp.loader.value)
        converter.item_index += 2
        value = self.converter.intern_string(value)
        return value

actions = make_table(StaticActionWriter, {
    0 : 'set_width',
    1 : 'set_scale'
})

conditions = make_table(StaticConditionWriter, {
})

expressions = make_table(StaticExpressionWriter, {
    0 : 'get_width',
    1 : 'get_height',
    2 : '.vertical_tab',
    3 : Base64Decode
})

def get_object():
    return FontInfoObject