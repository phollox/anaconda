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
from collections import defaultdict

"""
struct AlterableInfo
{
    uint8 is_fp[26]; // 0 if not set, 1 if int, 2 if fp 
};

struct ObjectInfo
{
    uint32 len;
    char name[len];
    AlterableInfo alt;
};

struct RunInfo
{
    uint32 count;
    ObjectInfo infos[count];
};
"""

def read_runinfo(data):
    info = {}
    reader = ByteReader(data)
    count = reader.readInt(True)
    for _ in xrange(count):
        name = reader.read(reader.readInt(True))
        alts = {}
        for i in xrange(26):
            alts[i] = reader.readByte(True)
        info[name] = alts
    return info

def write_runinfo(info):
    writer = ByteReader()
    writer.writeInt(len(info), True)
    for k, v in info.iteritems():
        writer.writeInt(len(k), True)
        writer.write(k)
        for i in xrange(26):
            reader.writeByte(v[i], True)

class RunInfo(object):
    def __init__(self, converter, filename):
        self.converter = converter
        self.objects = {}
        self.qualifiers = defaultdict(dict)
        try:
            with open(filename, 'rb') as fp:
                data = fp.read()
        except IOError:
            return
        self.info = read_runinfo(data)

        for game_index, game in enumerate(converter.games):
            qual_to_obj = defaultdict(list)
            for frameitem in game.frameItems.items:
                self.add_object(game_index, frameitem, self.qualifiers,
                                qual_to_obj)
            for qual, objs in qual_to_obj.iteritems():
                qual_alts = self.qualifiers[(qual, game_index)]
                for obj in objs:
                    self.info[obj.name].update(qual_alts)

    def get_qualifier(self, qualifier, game_index=None):
        if game_index is None:
            game_index = self.converter.game_index
        return self.qualifiers[(qualifier, game_index)]

    def add_object(self, game_index, frameitem, qualifiers, qual_to_obj):
        name = frameitem.name
        alts = self.info.get(name, None)
        if alts is None:
            return
        try:
            qualifier_keys = frameitem.properties.loader.qualifiers
        except AttributeError:
            return
        handle = (frameitem.handle, frameitem.objectType, game_index)
        self.objects[handle] = alts
        for qualifier in qualifier_keys:
            qual_to_obj[qualifier].append(frameitem)
            qual_alts = qualifiers[(qualifier, game_index)]
            for k, v in alts.iteritems():
                alt = qual_alts.get(k, 0)
                if v <= alt:
                    continue
                qual_alts[k] = v
