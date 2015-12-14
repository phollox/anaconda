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

from libc.stdio cimport FILE

class ByteReader:
    cdef unsigned char * buffer
    cdef unsigned int start, pos, data_size, lastPosition
    cdef bytes original
    cdef bint shared

    IF not IS_PYPY:
        cdef FILE * fp

    cdef object python_fp

    def tell(self)
    def data(self)
    def seek(self, int pos, int mode = ?)
    def read(self, int size = ?)
    def adjust(self, int to)
    def size(self)
    def readByte(self, bint asUnsigned = ?) except? -10
    def readShort(self, bint asUnsigned = ?) except? -10
    def readFloat(self) except? -10
    def readDouble(self) except? -10
    def readInt(self, bint asUnsigned = ?)
    def readString(self, size=?)
    def readUnicodeString(self, size=?)
    def readColor(self)
    def readReader(self, size_t size)
    def write(self, bytes data)
    def write_size(self, char * data, size_t size)
    def skipBytes(self, size_t n)
    def rewind(self, size_t n)
    cdef bint _read(self, void * value, int size) except False
