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

from mmfparser.data.chunkloaders.objectinfo import ObjectInfo
from mmfparser.loader import DataLoader
from mmfparser.bytereader import ByteReader

class FrameItems(DataLoader):
    # cdef public:
    #     dict itemDict

    def initialize(self):
        self.itemDict = {}
    
    @property
    def items(self):
        return self.itemDict.values()
    
    def read(self, reader):
        itemDict = self.itemDict
        for _ in xrange(reader.readInt(True)):
            item = self.new(ObjectInfo, reader)
            itemDict[item.handle] = item
    
    def fromHandle(self, handle):
        return self.itemDict[handle]
    
    def write(self, reader):
        reader.writeInt(len(self.items), True)
        for item in self.items:
            item.write(reader)

__all__ = ['FrameItems']