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
    StaticActionWriter, StaticExpressionWriter, make_table,
    EmptyAction, ActionMethodWriter, ExpressionMethodWriter)

class AdvancedDirection(ObjectWriter):
    class_name = 'AdvancedDirection'
    filename = 'advdir'

class SetObject(EmptyAction):
    def write(self, writer):
        obj = (self.parameters[0].loader.objectInfo,
               self.parameters[0].loader.objectType)
        self.converter.event_settings['movement_obj'] = obj

class ControllerAction(ActionMethodWriter):
    def get_object(self):
        obj = self.converter.event_settings.get('movement_obj', (None, None))
        return obj

class SetVectorSpeed(ControllerAction):
    method = 'get_movement()->set_speed'

class VectorLookAt(ControllerAction):
    method = 'get_movement()->look_at'

class GetDistance(ExpressionMethodWriter):
    method = 'get_distance'
    has_object = False

actions = make_table(StaticActionWriter, {
    51 : VectorLookAt,
    56 : SetVectorSpeed,
    74 : SetObject
})

conditions = make_table(StaticConditionWriter, {
})

expressions = make_table(StaticExpressionWriter, {
    56 : GetDistance,
    57 : 'get_object_angle'
})

def get_object():
    return AdvancedDirection
