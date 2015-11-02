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

import sys
sys.path.append('..')

from chowdren.platforms.generic import GenericPlatform
from chowdren.platforms.d3d import D3DPlatform
from chowdren.platforms.android import AndroidPlatform
from chowdren.platforms.html5 import HTML5Platform

classes = {
    'generic': GenericPlatform,
    'd3d': D3DPlatform,
    'android': AndroidPlatform,
    'html5': HTML5Platform
}

try:
    from extra import classes as extra_classes
    classes.update(extra_classes)
except ImportError, e:
    print 'Could not import all platforms'
    pass
