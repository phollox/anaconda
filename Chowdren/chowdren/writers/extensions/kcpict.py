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

from kcpica import ActivePicture
from chowdren.writers.events import (ComparisonWriter, ActionMethodWriter,
    ConditionMethodWriter, ExpressionMethodWriter, make_table)

class BackgroundPicture(ActivePicture):
    is_active_picture = False

    def is_static_background(self):
        return False

    def is_background_collider(self):
        # also:
        # ObstaclePlatform, both ObstaclePlatform and ObstascleSolid: Ladder
        return self.common.newFlags['ObstacleSolid']

actions = make_table(ActionMethodWriter, {
    0 : 'load',
    2 : 'set_visible(true)',
    3 : 'set_visible(false)',
})

conditions = make_table(ConditionMethodWriter, {
})

expressions = make_table(ExpressionMethodWriter, {
    0 : '.filename'
})

def get_object():
    return BackgroundPicture
