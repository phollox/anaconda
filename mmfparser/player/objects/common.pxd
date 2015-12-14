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

class BaseObject
class BackgroundPlayer
class ObjectPlayer

from mmfparser.player.collision cimport CollisionBase
from mmfparser.player.frame cimport Frame
from mmfparser.player.common cimport PlayerChild

class BaseObject(PlayerChild):
    cdef public:
        double x, y
        int width, height, actionX, actionY, hotspotX, hotspotY
        CollisionBase collision
        object frame
        object loader
        bint updateEnabled
    
    cdef void initialize(self, loader, frame)
    def update(self)
    def draw(self)
    def set_position(self, double x, double y)

class BackgroundPlayer(BaseObject):
    pass

class ObjectPlayer(BaseObject):
    cdef public:
        bint isGlobal