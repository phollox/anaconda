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

import imp
import functools
from configs import default

class ConfigurationFile(object):
    def __init__(self, converter, path=None):
        if path is None:
            module = default
        else:
            module = imp.load_source('config', path)
        for name in dir(default):
            if name.startswith('_'):
                continue
            func = getattr(module, name, None)
            if func is None:
                func = getattr(default, name)
            setattr(self, name, functools.partial(func, converter))
