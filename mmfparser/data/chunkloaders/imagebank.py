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
import zlib
from cStringIO import StringIO
from cffi import FFI
ffi = FFI()

from mmfparser.bytereader import ByteReader
from mmfparser.data import zlibdata
from mmfparser.data import onepointfive
from mmfparser.loader import DataLoader
from mmfparser.data.chunkloaders.appheader import graphicModes
# from mmfparser.common import allocate_memory

from mmfparser.bitdict import BitDict

POINT_MODE = 4 # 16 million colors
FIFTEENPOINT_MODE = 6  # 32768 colors
    
def indexImage(item, frame):
    if not item.indexed:
        return item.image
    else:
        image = item.image # <unsigned int *>(<char *>item.image)
        str_points = allocate_memory(len(item.image), new_points)
        new_points_array = new_points
        for i in xrange(len(item.image) / 4):
            current_value = frame.palette.items[image[i]]
            new_points_array[i] = (current_value[0] | current_value[1] << 8 | 
                                   current_value[2] << 16) 
        return str_points

def joinImage(points, alpha):
    c_array = ffi.new('unsigned int[]', len(points))
    for i in xrange(len(points) / 4):
        c_array[i] = points[i] | ((alpha)[i] << 24)
    return c_array

def getImageData(item, frame):
    points = indexImage(item, frame)
    points_c = points
    if item.alpha:
        data = joinImage(points, item.alpha)
    else:
        transparent = (item.transparent[0] | item.transparent[1] << 8 | 
            item.transparent[2] << 16)
        c_array = data = ffi.new('unsigned int[]', len(points_c))
        for i in xrange(len(points) / 4):
            if points_c[i] == transparent:
                c_array[i] = points_c[i]
            else:
                c_array[i] = points_c[i] | (0xFF << 24)
    return data

class BasePoint:
    pass

class Point(BasePoint):
    def __init__(self):
        self.size = 3

    def read(self, data, position):
        b = data[position]
        g = data[position+1]
        r = data[position+2]
        return r | g << 8 | b << 16

    def write(self, item, reader):
        reader.writeByte(item[2], True)
        reader.writeByte(item[1], True)
        reader.writeByte(item[0], True)

point_instance = Point()

class SixteenPoint(BasePoint):
    def __init__(self):
        self.size = 2

    def read(self, data, position):
        newShort = (data[position] | 
            data[position + 1] << 8)
        r = (newShort & 63488) >> 11
        g = (newShort & 2016) >> 5
        b = (newShort & 31)
        r = r << 3
        g = g << 2
        b = b << 3
        return r | g << 8 | b << 16

    def write(item, reader):
        r, g, b = item
        r = r >> 3
        g = g >> 2
        b = b >> 3
        r = r << 11
        g = g << 5
        return r | g | b

sixteen_point = SixteenPoint()

class FifteenPoint(BasePoint):
    def __init__(self):
        self.size = 2

    def read(self, data, position):
        newShort = (data[position] | data[position + 1] << 8)
        r = (newShort & 31744) >> 10
        g = (newShort & 992) >> 5
        b = (newShort & 31)
        r = r << 3
        g = g << 3
        b = b << 3
        return r | g << 8 | b << 16

    def write(item, reader):
        r, g, b = item
        r = r >> 3
        g = g >> 3
        b = b >> 3
        r = r << 10
        g = g << 5
        
        return r | g | b

fifteen_point = FifteenPoint()

class IndexPoint(BasePoint):
    def __init__(self):
        self.size = 1

    def read(self, data, position):
        return data[position]

    def write(self, point, reader):
        reader.writeByte(point, True)

index_point = IndexPoint()

def read_rgb(data, width, height, pointClass):
    c_array = ffi.new('unsigned int[]', width * height)
    # cdef x, y
    n = 0
    i = 0
    pad = get_padding(width, pointClass)
    for y in xrange(height):
        for x in xrange(width):
            c_array[i] = pointClass.read(data, n)
            n += pointClass.size
            i += 1
        n += pad * pointClass.size
    return c_array, n

def get_padding(width, pointClass, bytes = 2):
    pad = bytes - ((width * pointClass.size) % bytes)
    if pad == bytes:
        pad = 0
    # return ceil(pad / <float>pointClass.size)
    return (pad + pointClass.size - 1) / pointClass.size

def read_rle(data, width, height, pointClass):
    pad = get_padding(width, pointClass)
    c_array = ffi.new('unsigned int[]', width * height)
    currentPosition = 0
    i = 0
    pos = 0
    while 1:
        command = data[currentPosition]
        currentPosition += 1

        if command == 0:
            break

        if command > 128:
            command -= 128
            for n in xrange(command):
                if pos % (width + pad) < width:
                    c_array[i] = pointClass.read(data, currentPosition)
                    i += 1
                pos += 1
                currentPosition += pointClass.size
        else:
            newPoint = pointClass.read(data, currentPosition)
            for n in xrange(command):
                if pos % (width + pad) < width:
                    c_array[i] = newPoint
                    i += 1
                pos += 1
            currentPosition += pointClass.size
    return c_array, currentPosition

def read_alpha(data, width, height, position):
    pad = get_padding(width, index_point, 4)
    c_array = ffi.new('unsigned char[]', width * height)
    n = i = 0
    for y in xrange(height):
        for x in xrange(width):
            c_array[i] = data[n + position]
            n += 1
            i += 1
        n += pad
    return c_array

# NEW!!!

# cdef inline generate_alpha(item):
#     cdef pad = get_padding(item.width, index_point, 4)
#     cdef i, n, x, y
#     cdef alpha = item.alpha
#     cdef buf
#     points = allocate_memory(item.width * item.height + pad * item.height, &buf)
#     i = n = 0
#     for y in range(item.height):
#         for x in range(item.width):
#             buf[n] = alpha[i]
#             i += 1
#             n += 1
#         n += pad
#     return points
    
# cdef inline generate_image(item):
#     cdef x, y
#     cdef n = 0
#     cdef i = 0
#     cdef image = item.image
#     cdef pad = get_padding(item.width, point_instance)
#     cdef buf
#     points = allocate_memory((item.width * item.height + pad * item.height
#         ) * point_instance.size, &buf)
#     for y in range(item.height):
#         for x in range(item.width):
#             buf[n] = image[i+2]
#             buf[n+1] = image[i+1]
#             buf[n+2] = image[i]
#             i += 4
#             n += 3
#         n += pad * point_instance.size
#     return points

# OLD!!!

# def generate_alpha(item):
    # cdef pad = get_padding(item.width, index_point, 4)
    # data = ''
    # cdef i, x, y
    # alpha = item.alpha
    # i = 0
    # for y in range(item.height):
        # for x in range(item.width):
            # data += alpha[i]
            # i += 1
        # data += '\x00' * pad
    # return data
    
# def generate_image(item):
    # cdef x, y
    # cdef n = 0
    # cdef i = 0
    # image = item.image
    # data = ''
    # cdef pad = get_padding(item.width, point_instance)
    # for y in range(item.height):
        # for x in range(item.width):
            # data += image[i+2]
            # data += image[i+1]
            # data += image[i]
            # i += 4
        # data += '\x00' * (pad * point_instance.size)
    # return data

IMAGE_FLAGS = BitDict(
    'RLE',
    'RLEW',
    'RLET',
    'LZX',
    'Alpha',
    'ACE',
    'Mac'
)

class ImageItem(DataLoader):
    __slots__ = ['handle', 'checksum', 'references', 'width', 'height',
                 'xHotspot', 'yHotspot', 'actionX', 'actionY', 'flags',
                 'image', 'alpha', 'indexed', 'graphicMode', 'transparent',
                 'reader', 'pos']

    def initialize(self):
        self.flags = IMAGE_FLAGS.copy()

    def read(self, reader):
        self.handle = reader.readInt()
        load_now = self.settings.get('loadImages', True)
        self.reader = reader
        self.pos = reader.tell()
        if load_now:
            self.load()
            return

        old = self.settings.get('old', False)
        if old:
            raise NotImplementedError('not supported')

        debug = self.settings.get('debug', False)
        if debug:
            reader.skipBytes(8)
            size = reader.readInt(True)
            reader.skipBytes(size + 20)
        else:
            reader.skipBytes(4)
            size = reader.readInt(True)
            reader.skipBytes(size)

    def load(self):
        reader = self.reader
        if reader is None:
            return
        reader.seek(self.pos)
        self.reader = None

        old = self.settings.get('old', False)
        debug = self.settings.get('debug', False)
        if old:
            newReader = onepointfive.decompress(reader)
        elif debug:
            newReader = reader
        else:
            newReader = zlibdata.decompress(reader)

        start = newReader.tell()
        
        if old:
            self.checksum = newReader.readShort()
        else:
            self.checksum = newReader.readInt()
        self.references = newReader.readInt()
        size = newReader.readInt(True)
        
        if debug:
            newReader = newReader.readReader(size + 20)
        self.width = newReader.readShort()
        self.height = newReader.readShort()
        self.graphicMode = newReader.readByte()
        self.flags.setFlags(newReader.readByte(True))
        
        if not old:
            newReader.skipBytes(2) # imgNotUsed
        self.xHotspot = newReader.readShort()
        self.yHotspot = newReader.readShort()
        self.actionX = newReader.readShort()
        self.actionY = newReader.readShort()
        if old:
            self.transparent = (0, 0, 0)
        else:
            self.transparent = newReader.readColor()

        if self.flags['LZX']:
            decompressed = newReader.readInt()
            newReader = ByteReader(zlib.decompress(newReader.read()))

        width, height = self.width, self.height
        if self.graphicMode == 2:
            pointClass = index_point
            self.indexed = True
        elif self.graphicMode == 3:
            pointClass = index_point
            self.indexed = True
        elif self.graphicMode == 4: # 16 million colors
            pointClass = point_instance
            self.indexed = False
        elif self.graphicMode == 6: # 32768 colors
            pointClass = fifteen_point
            self.indexed = False
        elif self.graphicMode == 7: # 65536 colors
            pointClass = sixteen_point
            self.indexed = False
        else:
            import code
            code.interact(local = locals())
            newReader.openEditor()
            raise NotImplementedError('unknown graphic mode: %s'
                                      % self.graphicMode)

        readerData = newReader.read()
        data = bytearray(readerData)
        if self.flags['RLE'] or self.flags['RLEW'] or self.flags['RLET']:
            image, bytesRead = read_rle(data, width, height, pointClass)
            alphaSize = size - bytesRead
        else:
            image, imageSize = read_rgb(data, width, height, pointClass)
            alphaSize = size - imageSize
        self.image = image

        if self.flags['Alpha']:
            pad = (alphaSize - width * height) / height
            self.alpha = read_alpha(data, width, height, size - alphaSize)
        else:
            self.alpha = None
    
    def write(self, reader):
        debug = self.settings.get('debug', False)
        dataReader = ByteReader()

        dataReader.write(generate_image(self))
        if self.alpha is not None:
            dataReader.write(generate_alpha(self))

        newReader = ByteReader()
        newReader.writeInt(self.checksum)
        newReader.writeInt(self.references)
        newReader.writeInt(len(dataReader))
        newReader.writeShort(self.width)
        newReader.writeShort(self.height)
        newReader.writeByte(4)#self.graphicMode)
        # XXX simple hack
        if self.flags['Alpha']:
            newReader.writeByte(16)
        else:
            newReader.writeByte(0)
        newReader.write(b'\x00\x00')
        newReader.writeShort(self.xHotspot)
        newReader.writeShort(self.yHotspot)
        newReader.writeShort(self.actionX)
        newReader.writeShort(self.actionY)
        newReader.writeColor(self.transparent or (0, 0, 0))
        newReader.writeReader(dataReader)

        reader.writeInt(self.handle)
        if debug:
            reader.writeReader(newReader)
        else:
            reader.writeReader(zlibdata.compress(newReader))
    
    def getImageData(self, frame = None, **kw):
        image = getImageData(self, frame)
        self.unload()
        return ffi.buffer(image)[:]

    def unload(self):
        self.image = None
        self.alpha = None
    
    def getGraphicMode(self):
        return graphicModes[self.graphicMode]

class JavaImage(DataLoader):
    __slots__ = ['handle', 'xHotspot', 'yHotspot', 'actionX', 'actionY',
                 'width', 'height', 'data', 'flash']

    def read(self, reader):
        self.handle = reader.readShort()
        if self.settings.get('withSize', False):
            self.width = reader.readShort()
            self.height = reader.readShort()
        self.xHotspot = reader.readShort()
        self.yHotspot = reader.readShort()
        self.actionX = reader.readShort()
        self.actionY = reader.readShort()
        size = reader.readInt()
        data = reader.read(size)
        try:
            self.data = zlib.decompress(data)
            self.flash = True
        except zlib.error:
            self.data = data
            self.flash = False
    
    def write(self, reader):
        reader.writeShort(self.handle)
        if self.settings.get('withSize', False):
            reader.writeShort(self.width)
            reader.writeShort(self.height)
        reader.writeShort(self.xHotspot)
        reader.writeShort(self.yHotspot)
        reader.writeShort(self.actionX)
        reader.writeShort(self.actionY)
        reader.writeInt(len(self.data))
        reader.write(self.data)
    
    def createDisplay(self, *arg, **kw):
        if self.flash:
            from mmfparser.player.sprite import ImageData
            newImage = ImageData(self.width, self.height, 'ARGB', 
                self.data, self.width * -4, None)
        else:
            from mmfparser.player.common import load_image
            newImage = load_image(StringIO(self.data))
        newImage.anchor_x = self.xHotspot
        newImage.anchor_y = self.height - self.yHotspot
        return newImage

    def getImageData(self, *arg, **kw):
        from mmfparser.image import Image
        img = Image(data = self.data)
        return img.get_data()

class ImageBank(DataLoader):
    __slots__ = ['itemDict']
    
    @property
    def items(self):
        return self.itemDict.values()
    
    def initialize(self):
        self.itemDict = {}

    def read(self, reader):
        java = self.settings.get('java', False)
        build = self.settings['build']
        if not java:
            if self.settings.get('debug', False):
                path = self.readString(reader)
                newReader = ByteReader(open(path, 'rb'))
                newReader.skipBytes(4)
                bank = self.new(AGMIBank, newReader)
                for item in bank.items:
                    self.itemDict[item.handle] = item
                return
            numberOfItems = reader.readInt()
            for i in range(numberOfItems):
                newItem = self.new(ImageItem, reader)
                if build >= 284:
                    newItem.handle -= 1
                self.itemDict[newItem.handle] = newItem
        
        else:
            numberOfItems = reader.readShort()
            itemsInBank = reader.readShort()
            itemsToRead = itemsInBank

            currentPosition = reader.tell()
            try:
                self.itemDict = itemDict = {}
                for _ in xrange(itemsToRead):
                    newItem = self.new(JavaImage, reader, withSize = True)
                    itemDict[newItem.handle] = newItem
            except struct.error:
                reader.seek(currentPosition)
                self.itemDict = itemDict = {}
                for _ in xrange(itemsToRead):
                    newItem = self.new(JavaImage, reader, withSize = False)
                    itemDict[newItem.handle] = newItem
    
    def write(self, reader):
        java = self.settings.get('java', False)
        if not java:
            offsetDict = {}
            self.offsets = offsets = []
            reader.writeInt(len(self.items))
            for item in self.items:
                offsetDict[item.handle] = reader.tell() + 4
                item.write(reader)
            for key in sorted(offsetDict.keys()):
                offsets.append(offsetDict[key])
        else:
            reader.writeShort(len(self.items))
            reader.writeShort(len(self.items))
            for item in self.items:
                item.write(reader)
    
    def fromHandle(self, handle):
        return self.itemDict[handle]

from mmfparser.data.mfaloaders.imagebank import AGMIBank

__all__ = ['ImageBank', 'JavaImage', 'ImageItem']