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

from chowdren.writers.events import (StaticActionWriter, StaticConditionWriter,
    StaticExpressionWriter, make_table)

class ModFusionEX(ObjectWriter):
    class_name = 'ModFusion'
    filename = 'modfusion'
    static = True
    defines = ['CHOWDREN_USE_MODFUSION']

    def write_init(self, writer):
        pass

actions = make_table(StaticActionWriter, {
    2 : 'load_track',
    3 : 'clear_cache',
    5 : 'load_wgm',
    7 : 'load_wgm',
    11 : 'play',
    12 : 'stop',
    13 : 'stop_fade',
    15 : 'set_volume',
    21 : 'enable_loop',
    22 : 'disable_loop',
})

conditions = make_table(StaticConditionWriter, {
    1 : 'is_playing'
})

expressions = make_table(StaticExpressionWriter, {
    2 : 'get_volume'
})

def get_object():
    return ModFusionEX
