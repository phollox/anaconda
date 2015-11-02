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

class Viewport(ObjectWriter):
    class_name = 'Viewport'
    filename = 'viewport'
    defines = ['CHOWDREN_USE_VIEWPORT']
    use_alterables = True

    def write_init(self, writer):
        data = self.get_data()
        data.skipBytes(4)
        width = data.readShort()
        height = data.readShort()
        writer.putln('src_width = width = %s;' % width)
        writer.putln('src_height = height = %s;' % height)
        writer.putln('center_x = x + width / 2;')
        writer.putln('center_y = y + height / 2;')

actions = make_table(ActionMethodWriter, {
    0 : 'set_width',
    1 : 'set_height',
    3 : 'set_source'
})

conditions = make_table(ConditionMethodWriter, {
})

expressions = make_table(ExpressionMethodWriter, {
})

def get_object():
    return Viewport
