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

from mmfparser.bytereader import ByteReader
import sys
from PIL import Image

def to_png(data):
    reader = ByteReader(data)
    width = reader.readInt(True)
    height = reader.readInt(True)
    data = reader.read(width*height)
    return Image.frombytes('L', (width, height), data)

def main():
    with open(sys.argv[1], 'rb') as fp:
        data = fp.read()
    image = to_png(data)
    image.save(sys.argv[2])

if __name__ == '__main__':
    main()