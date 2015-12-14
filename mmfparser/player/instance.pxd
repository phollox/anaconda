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

class Instance

from mmfparser.player.alterables cimport Alterables
from mmfparser.player.objects.common cimport BaseObject
from mmfparser.player.frame cimport Frame, Layer
from mmfparser.player.movements.common cimport MovementPlayer
from mmfparser.player.common cimport PlayerChild

class InkEffect:
    cdef public:
        object effect
        object shaders
        object parameters

    cdef void begin(self)
    cdef void end(self)

class Instance(PlayerChild):
    cdef public:
        Frame frame
        double x, y
        int x1, y1, x2, y2
        int oldX1, oldY1, oldX2, oldY2
        bint boxSet
        bint collisionSet
        int direction
        object loader
        object objectInstance
        int handle
        object objectInfo
        int objectType
        BaseObject objectPlayer
        list movementPlayers
        MovementPlayer currentMovement
        int movementIndex
        bint movementEnabled
        bint updateEnabled
        bint sleep
        bint destroyed
        bint destroying
        bint visible
        bint scroll
        bint flashing
        InkEffect inkEffect
        double transparency
        Layer layer
        int layerIndex
        Alterables alterables
        bint wrap
        bint wrapUp
        bint wrapDown
        bint wrapLeft
        bint wrapRight
        bint outsideKill
        bint positionChanged
        bint initialized
        tuple colorCoefficient

    cdef void update_bounding_box(self)
    def set_position(self, double x, double y, bint fromAction = ?)
    def update_collisions(self, list instances)
    def set_direction(self, int direction, bint force = ?)
    def inline int get_direction(self)
    def inline int get_leaving_corners(self)
    def inline int get_entering_corners(self)
    def inline bint is_leaving(self)
    def inline bint outside_playfield(self)
    def inline tuple get_outside_corners(self)
    def inline bint in_playfield(self, int x_border = ?, int y_border = ?)
    def inline bint inside_window(self, int x_border = ?, int y_border = ?)
    def inline bint in_zone(self, int xZone, int yZone, int xZone2, 
        int yZone2)
    def inline bint inside_zone(self, int xZone, int yZone, int xZone2, 
        int yZone2)
    def inline bint mouse_over(self)
    cdef void draw(self)
    def object_changed(self)
    cdef void update(self)
    cdef void update_playfield(self)