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

from chowdren.platforms.generic import GenericPlatform
from chowdren.shader import translate_shader_data
from mmfparser.bytereader import ByteReader

class HTML5Platform(GenericPlatform):
    def get_shader(self, name, vert, frag):
        vert = translate_shader_data(vert, 'vertex', 'gles')
        frag = translate_shader_data(frag, 'fragment', 'gles')
        writer = ByteReader()
        writer.writeInt(len(vert))
        writer.write(vert)
        writer.writeInt(len(frag))
        writer.write(frag)
        return str(writer)

    def install(self):
        pass
