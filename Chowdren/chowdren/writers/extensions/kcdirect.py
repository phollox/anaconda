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

from chowdren.writers.events import (ActionMethodWriter, ConditionMethodWriter,
    ExpressionMethodWriter, make_table)

class DirectionCalculator(ObjectWriter):
    class_name = 'DirectionCalculator'
    static = True

class LookAt(ActionMethodWriter):
    method = 'look_at'

    def get_object(self):
        loader = self.parameters[0].loader
        return loader.objectInfo, loader.objectType

    def write(self, writer):
        details = self.convert_index(1)
        if not details.get('parent'):
            raise NotImplementedError()
        obj = self.converter.get_object(details['parent'])
        x = '%s->get_x() + %s' % (obj, details['x'])
        y = '%s->get_y() + %s' % (obj, details['y'])
        writer.put('look_at(%s, %s);' % (x, y))

class RotateToward(ActionMethodWriter):
    method = 'rotate_toward'

    def write(self, writer):
        direction = self.convert_index(0)
        writer.putc('rotate_toward(%s);', direction)

    def get_object(self):
        loader = self.parameters[1].loader
        return loader.objectInfo, loader.objectType

actions = make_table(ActionMethodWriter, {
    1: RotateToward,
    2 : LookAt
})

conditions = make_table(ConditionMethodWriter, {
})

expressions = make_table(ExpressionMethodWriter, {
})

def get_object():
    return DirectionCalculator