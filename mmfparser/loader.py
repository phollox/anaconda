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

class DataLoader:
    def __init__(self, reader = None, parent = None, **settings):
        self.init(reader, parent, settings)

    def init(self, reader, parent, settings):
        self.parent = parent
        self.settings = settings
        self.initialize()
        if reader is not None:
            self.read(reader)
        return True

    def new(self, loaderClass, reader = None, **kw):
        kw.update(self.settings)
        newLoader = loaderClass()
        newLoader.init(reader, self, kw)
        return newLoader

    def readString(self, reader, size=None):
        if self.settings.get('unicode', False):
            return reader.readUnicodeString(size).encode('utf-8')
        else:
            return reader.readString(size)

    def initialize(self):
        return

    def read(self, reader):
        raise NotImplementedError('%s has not implemented a read method' %
            self.__class__.__name__)

    def generate(self):
        newReader = ByteReader()
        self.write(newReader)
        return newReader

    def write(self, reader):
        raise NotImplementedError('%s has not implemented a write method' %
            self.__class__.__name__)