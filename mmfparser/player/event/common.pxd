# Copyright (c) Mathias Kaerlev 2012.

# This file is part of Anaconda.

# Anaconda is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# Anaconda is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with Anaconda.  If not, see <http://www.gnu.org/licenses/>.

class ACEBase
class ACBase
class BaseParameter
class ValueParameter
class BaseExpression
class ExpressionList
class ConstantExpressionList

from mmfparser.player.frame cimport Frame
from mmfparser.player.event.player cimport Group, EventPlayer
from mmfparser.player.instance cimport Instance
from mmfparser.player.common cimport PlayerChild
from mmfparser.player.event.evaluater cimport Evaluater
from mmfparser.player.event.expressions.common cimport Expression

from cpython cimport PyObject, Py_INCREF, Py_DECREF

cimport cython

class ACEBase(PlayerChild):
    cdef public:
        object loader
        bint _iterateObjects
        Frame frame
        Group group
        EventPlayer eventPlayer
        object objectInfo
        list resolvedObjects
        Evaluater evaluater

    cdef void _initialize(self, loader)
    cdef void created(self)
    def Instance get_instance(self, objectInfo = ?)
    def list get_instances(self, objectInfo = ?)
    def list resolve_objects(self, objectInfo)
    def list get_all_instances(self, objectInfo = ?)
    def select_instances(self, list instanceList, objectInfo = ?)
    def get_frame_instances(self)
    def select_frame_instances(self, list instanceList, list allInstances = ?)
    def evaluate_expression(self, object loader)
    def evaluate_index(self, int index)
    
    # parameter stuff
    def direction_from(self, x1, y1, x2, y2, f = ?)
    def get_time(self, parameter)
    def get_filename(self, parameter)
    def get_color(self, parameter)
    def get_alterable_index(self, parameter)
    def get_global_index(self, parameter)
    def get_parameter_value(self, parameter)
    def get_direction(self, parameter)
    def list get_directions(self, parameter)
    def list get_positions(self, position)

class BaseParameter(PlayerChild):
    cdef public bint isExpression
    cdef void initialize(self, loader)

class ValueParameter(BaseParameter):
    cdef public object value

class BaseExpression(BaseParameter):
    pass

class ExpressionList(BaseExpression):
    cdef:
        int size
        PyObject ** items

class ConstantExpressionList(BaseExpression):
    cdef public object value

class ACBase(ACEBase):
    cdef public:
        list parameters
        
    def get_parameter(self, int index)