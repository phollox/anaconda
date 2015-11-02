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
    StaticActionWriter, StaticExpressionWriter, make_table)

class Util(ObjectWriter):
    class_name = 'Utility'
    static = True

    def write_init(self, writer):
        pass

actions = make_table(StaticActionWriter, {
    1 : 'SetRandomSeedToTimer'
})

conditions = make_table(StaticConditionWriter, {
})

expressions = make_table(StaticExpressionWriter, {
    0 : 'IntGenerateRandom',
    1 : 'GenerateRandom',
    3 : 'Substr',
    4 : 'Nearest',
    6 : 'ModifyRange',
    2 : 'Limit',
    13 : 'IntNearest',
    15 : 'IntModifyRange',
    21 : 'ExpressionCompare',
    22 : 'IntExpressionCompare',
    23 : 'StrExpressionCompare',
    8 : 'EuclideanMod',
    12 : 'IntLimit',
    24 : 'Approach',
    18 : 'IntUberMod',
    7 : 'Wave',
    9 : 'UberMod',
    11 : 'Mirror',
    17 : 'IntEuclideanMod',
    19 : 'IntInterpolate',
    25 : 'IntApproach',
    16 : 'IntWave',
    10 : 'Interpolate'
})

def get_object():
    return Util