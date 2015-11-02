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

from cStringIO import StringIO
from mmfparser.bytereader import ByteReader
from mmfparser.webp import encode
from mmfparser import zopfli
import zlib
import sys

class Platform(object):
    save_dir = '.'

    def __init__(self, converter):
        self.converter = converter
        self.initialize()
        if hasattr(sys, 'frozen'):
            self.compress = zlib.compress
        else:
            self.compress = zopfli.compress
        self.problem_images = 0

    def get_image(self, image):
        if self.converter.config.use_webp():
            webp = encode(image.tobytes('raw', 'RGBA'),
                          image.size[0], image.size[1])
            return webp
        return self.compress(image.tobytes(), 9)

    def get_shader(self, name, vertex, fragment):
        raise NotImplementedError()

    def initialize(self):
        pass

    def install(self):
        pass

    def get_sound(self, name, data):
        return name, data
