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

from chowdren.common import make_color

FADE_TYPES = {
    'DOOR': 'DOOR',
    'FADE': 'FADE'
}

def write(writer, fade, out):
    fade_dir = 1.0 / (fade.duration / 1000.0)
    if not out:
        fade_dir = -fade_dir
    color = fade.color
    fade_type = 'Transition::%s' % FADE_TYPES.get(fade.name, 'FADE')
    writer.putlnc('manager.set_fade(%s, %s, %s);', fade_type,
                  make_color(color), fade_dir)
