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

import struct
import subprocess
import os
import traceback
import sys
import tempfile
import array

BYTE = struct.Struct('b')
UBYTE = struct.Struct('B')
SHORT = struct.Struct('h')
USHORT = struct.Struct('H')
FLOAT = struct.Struct('f')
DOUBLE = struct.Struct('d')
INT = struct.Struct('i')
UINT = struct.Struct('I')

def check_available(reader, size):
    if reader.pos + size > reader.data_size:
        import traceback
        traceback.print_stack()
        raise struct.error('%s bytes required' % size)
    return 0

class ByteReader:
    def __init__(self, input = None, start = None, size = None):
        self.pos = 0
        if isinstance(input, file):
            self.python_fp = input
            self.shared = False
            self.start = 0
            return
        else:
            self.python_fp = None
            if input is not None:
                data = input
            else:
                data = array.array('B')
        isChild = start is not None and size is not None
        self.shared = isChild
        self.original = data
        if isChild:
            self.start = start
            data = buffer(data, start)
        else:
            self.start = 0
        if isChild:
            self.data_size = size
        else:
            self.data_size = len(data)

        self.buffer = data

    def tell(self):
        if self.python_fp:
            return self.python_fp.tell()
        return self.pos

    def data(self):
        if self.python_fp:
            pos = self.tell()
            self.seek(0)
            data = self.read()
            self.seek(pos)
            return data

        return str(self.buffer[0:self.data_size])

    def seek(self, pos, mode = 0):
        if self.python_fp:
            self.python_fp.seek(pos, mode)
            return

        if mode == 2:
            pos += self.data_size
        elif mode == 1:
            pos += self.pos
        if pos > self.data_size:
            pos = self.data_size
        if pos < 0:
            pos = 0
        self.pos = pos
        return True

    def adjust(self, to):
        value = to - (self.tell() % to)

        if self.python_fp:
            self.seek(self.tell() + value)
            return

        self.pos += value

    def _read(self, size):
        if self.python_fp:
            data = self.python_fp.read(size)
            if len(data) < size:
                raise struct.error('%s bytes required' % size)
            return data

        check_available(self, size)
        ret = buffer(self.buffer, self.pos, size)
        self.pos += size
        return ret

    def readStruct(self, s):
        data = self._read(s.size)
        return s.unpack(data)[0]

    def read(self, size = -1):
        if self.python_fp:
            if size == -1:
                size = self.size() - self.tell()
            return self.python_fp.read(size)

        if size == -1 or size + self.pos > self.data_size:
            size = self.data_size - self.pos
        if size < 0:
            size = 0
        data = self.buffer[self.pos:self.pos+size]
        self.pos += size
        if self.pos > self.data_size:
            self.pos = self.data_size
        return data

    def size(self):
        if self.python_fp:
            pos = self.tell()
            self.seek(0, 2)
            size = self.tell()
            self.seek(pos)
            return size
        
        return self.data_size

    def __len__(self):
        ret = self.size()
        return int(ret)

    def __str__(self):
        return self.data()

    def __repr__(self):
        return repr(str(self))

    def readByte(self, asUnsigned = False):
        return self.readStruct(UBYTE if asUnsigned else BYTE)

    def readShort(self, asUnsigned = False):
        return self.readStruct(USHORT if asUnsigned else SHORT)

    def readFloat(self):
        return self.readStruct(FLOAT)

    def readDouble(self):
        return self.readStruct(DOUBLE)

    def readInt(self, asUnsigned = False):
        return self.readStruct(UINT if asUnsigned else INT)

    def readString(self, size=None):
        if size is not None:
            return self.readReader(size).readString()
        data = ''
        while 1:
            c = self.read(1)
            if c in ('\x00', ''):
                break
            data += c
        return data

    def readUnicodeString(self, size=None):
        if size is not None:
            return self.readReader(size*2).readUnicodeString()

        currentPosition = self.tell()
        data = ''
        while 1:
            endPosition = self.tell()
            c = self.read(2)
            if len(c) != 2:
                break
            if c == '\x00\x00':
                break
            data += c

        return data.decode('utf-16-le')

    def readColor(self):
        currentPosition = self.tell()
        r = self.readByte(True)
        g = self.readByte(True)
        b = self.readByte(True)
        self.skipBytes(1)
        return (r, g, b)

    def readReader(self, size):
        if self.python_fp:
            data = self.read(size)
            reader = ByteReader(data, 0, len(data))
            return reader

        check_available(self, size)
        self.shared = True
        reader = ByteReader(self.original, self.pos + self.start, size)
        self.pos += size
        return reader

    def readIntString(self):
        length = self.readInt(True)
        return self.read(length)

    def write(self, data):
        size = len(data)
        if size == 0:
            return False

        if self.python_fp:
            self.python_fp.write(data)
            return True

        self.buffer.fromstring(data)
        self.pos += size
        return True

    # def write_size(self, data, size):
    #     if size == 0:
    #         return False

    #     if self.python_fp:
    #         self.python_fp.write(buffer(data, 0, size))
    #         return True

    #     self.data += buffer(data
    #     self.pos += size
    #     return True

    def writeByte(self, value, asUnsigned = False):
        self.writeStruct(UBYTE if asUnsigned else BYTE, value)

    def writeShort(self, value, asUnsigned = False):
        self.writeStruct(USHORT if asUnsigned else SHORT, value)

    def writeFloat(self, value):
        self.writeStruct(FLOAT, value)

    def writeDouble(self, value):
        self.writeStruct(DOUBLE, value)

    def writeInt(self, value, asUnsigned = False):
        self.writeStruct(UINT if asUnsigned else INT, value)

    def writeString(self, value, size = -1):
        if size == -1:
            self.write(value + "\x00")
        else:
            for i in range(size):
                self.writeByte(0)
            currentPosition = self.tell()
            self.rewind(size)
            self.write(value[:size-1])
            self.seek(currentPosition)

    def writeUnicodeString(self, value):
        self.write(value.encode('utf-16-le') + "\x00\x00")

    def writeColor(self, colorTuple):
        r, g, b = colorTuple
        self.writeByte(r, True)
        self.writeByte(g, True)
        self.writeByte(b, True)
        self.writeByte(0)

    def writeFormat(self, format, *values):
        self.write(struct.pack(format, *values))

    def writeStruct(self, structType, *values):
        self.write(structType.pack(*values))

    def writeReader(self, reader):
        self.write(reader.data())

    def writeIntString(self, value):
        self.writeInt(len(value), True)
        self.write(value)

    def skipBytes(self, n):
        self.seek(n, 1)

    def rewind(self, n):
        self.seek(-n, 1)

    def truncate(self, value):
        self.buffer.truncate(value)

    def checkDefault(self, value, *defaults):
        return checkDefault(self, value, *defaults)

    def openEditor(self):
        if self.python_fp:
            name = self.python_fp.name
        else:
            fp = tempfile.NamedTemporaryFile('wb', delete = False)
            fp.write(self.data())
            fp.close()
            name = fp.name

        try:
            raw_input('Press enter to open hex editor...')
            openEditor(name, self.tell())
        except IOError:
            pass
        raw_input('(enter)')

def openEditor(filename, position):
    return subprocess.Popen(['010editor', '%s@%s' % (filename, position)])

def checkDefault(reader, value, *defaults):
    if value in defaults:
        return False
    # lastPosition = reader.lastPosition
    # size = reader.tell() - lastPosition
    lastPosition = reader.tell()
    size = -1
    reprDefaults = defaults
    if len(defaults) == 1:
        reprDefaults, = defaults
    message = ('unimplemented value at %s, size %s (should be '
               '%s but was %s)' % (lastPosition, size, reprDefaults, value))
    traceback.print_stack(file=sys.stdout)
    print message
    if sys.stdin.isatty():
        reader.openEditor()
    raise SystemExit