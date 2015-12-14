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

from mmfparser.loader import DataLoader
from mmfparser.bytereader import ByteReader
from mmfparser.bytereader import checkDefault
from mmfparser.bitdict import BitDict
from mmfparser.data.chunkloaders import (actions, expressions,
    conditions)
from mmfparser.data.chunkloaders.parameters.loaders import (parameterLoaders,
    getName)
from mmfparser.data.chunkloaders.common import _ObjectInfoMixin
from mmfparser.data.chunkloaders.common import _AceCommon

HEADER = 'ER>>'
EVENT_COUNT = 'ERes'
EVENTGROUP_DATA = 'ERev'
END = '<<ER'

ACE_FLAGS = BitDict(
    'Repeat',
    'Done',
    'Default',
    'DoneBeforeFadeIn',
    'NotDoneInStart',
    'Always',
    'Bad',
    'BadObject',
    None,
    'Notable'
)

ACE_OTHERFLAGS = BitDict(
    'Not',
    'Notable',
    'Monitorable',
    'ToDelete',
    'NewSound'
)

GROUP_FLAGS = BitDict(
    'Once',
    'NotAlways',
    'Repeat',
    'NoMore',
    'Shuffle',
    'EditorMark',
    'UndoMark',
    'ComplexGroup',
    'Breakpoint',
    'AlwaysClean',
    'OrInGroup',
    'StopInGroup',
    'OrLogical',
    'Grouped',
    'Inactive',
    'NoGood'
)

class Qualifier(DataLoader, _ObjectInfoMixin):
    objectInfo = None
    type = None
    qualifier = None
    objects = None

    def read(self, reader):
        self.objectInfo = reader.readShort(True)
        self.type = reader.readShort()
        self.qualifier = self.getQualifier()

    def resolve_objects(self, frameItems):
        if self.objects:
            return self.objects
        objects = self.objects = []
        for item in frameItems.items:
            try:
                if not self.qualifier in item.properties.loader.qualifiers:
                    continue
                if item.objectType != self.type:
                    continue
                objects.append(item.handle)
            except AttributeError:
                pass
        return objects

    def write(self, reader):
        reader.writeShort(self.objectInfo, True)
        reader.writeShort(self.type)

class Parameter(DataLoader):
    # cdef public:
    #     int code
    #     object loader

    def read(self, reader):
        currentPosition = reader.tell()
        size = reader.readShort()
        self.code = reader.readShort()
        if self.code >= len(parameterLoaders):
            print 'Unknown parameter code:', self.code
        self.loader = self.new(parameterLoaders[self.code], reader)
        reader.seek(currentPosition+size)

    def getName(self):
        return getName(self.code)

    def write(self, reader):
        newReader = ByteReader()
        newReader.writeShort(self.code)
        self.loader.write(newReader)
        reader.writeShort(len(newReader) + 2, True)
        reader.writeReader(newReader)

class Action(_AceCommon):
    # cdef public:
    #     object flags, otherFlags
    #     int defType
    #     list items

    def initialize(self):
        self.systemDict = actions.systemDict
        self.extensionDict = actions.extensionDict
        self.flags = ACE_FLAGS.copy()
        self.otherFlags = ACE_OTHERFLAGS.copy()

    def read(self, reader):
        currentPosition = reader.tell()
        size = reader.readShort(True)
        self.objectType = reader.readShort()
        self.num = reader.readShort()
        self.objectInfo = reader.readShort(True)
        self.objectInfoList = reader.readShort()
        self.flags.setFlags(reader.readByte(True))
        self.otherFlags.setFlags(reader.readByte(True))
        numberOfParameters = reader.readByte()
        self.defType = reader.readByte()
        self.items = [self.new(Parameter, reader)
            for i in range(numberOfParameters)]
        reader.seek(currentPosition+size)

    def write(self, reader):
        newReader = ByteReader()
        newReader.writeShort(self.objectType)
        newReader.writeShort(self.num)
        newReader.writeShort(self.objectInfo, True)
        newReader.writeShort(self.objectInfoList)
        newReader.writeByte(self.flags.getFlags(), True)
        newReader.writeByte(self.otherFlags.getFlags(), True)
        newReader.writeByte(len(self.items))
        newReader.writeByte(self.defType)

        for item in self.items:
            item.write(newReader)

        reader.writeShort(len(newReader) + 2, True)
        reader.writeReader(newReader)

    def getExtensionNum(self):
        return self.num - 80

class Condition(_AceCommon):
    flags = None
    otherFlags = None
    defType = 0
    identifier = 0
    items = None

    def initialize(self):
        self.systemDict = conditions.systemDict
        self.extensionDict = conditions.extensionDict
        self.flags = ACE_FLAGS.copy()
        self.otherFlags = ACE_OTHERFLAGS.copy()

    def read(self, reader):
        currentPosition = reader.tell()
        size = reader.readShort(True)
        self.objectType = reader.readShort()
        self.num = reader.readShort()
        self.objectInfo = reader.readShort(True)
        self.objectInfoList = reader.readShort()
        self.flags.setFlags(reader.readByte(True))
        self.otherFlags.setFlags(reader.readByte(True))
        numberOfParameters = reader.readByte()
        self.defType = reader.readByte()
        self.identifier = reader.readShort() # Event identifier

        self.items = [self.new(Parameter, reader)
            for i in range(numberOfParameters)]

        reader.seek(currentPosition + size)

    def write(self, reader):
        newReader = ByteReader()
        newReader.writeShort(self.objectType)
        newReader.writeShort(self.num)
        newReader.writeShort(self.objectInfo, True)
        newReader.writeShort(self.objectInfoList)
        newReader.writeByte(self.flags.getFlags(), True)
        newReader.writeByte(self.otherFlags.getFlags(), True)
        newReader.writeByte(len(self.items), True)
        newReader.writeByte(self.defType)
        newReader.writeShort(self.identifier)

        for item in self.items:
            item.write(newReader)

        reader.writeShort(len(newReader) + 2, True)
        reader.writeReader(newReader)

    def getExtensionNum(self):
        return - self.num - 80 - 1

class EventGroup(DataLoader):
    # cdef public:
    #     object flags
    #     int is_restricted
    #     int restrictCpt
    #     int identifier
    #     int undo
    #     list conditions
    #     list actions

    def initialize(self):
        self.flags = GROUP_FLAGS.copy()

    def read(self, reader):
        currentPosition = reader.tell()
        size = reader.readShort()*-1

        numberOfConditions = reader.readByte(True)
        numberOfActions = reader.readByte(True)
        self.flags.setFlags(reader.readShort(True))

        compat = self.settings.get('compat', False)
        if self.settings['build'] >= 284 and not compat:
            reader.skipBytes(2)
            self.is_restricted = reader.readInt()
            self.restrictCpt = reader.readInt()
        else:
            self.is_restricted = reader.readShort() # If the group is inhibited
            self.restrictCpt = reader.readShort() # Counter
            self.identifier = reader.readShort() # Unique identifier
            self.undo = reader.readShort() # Identifier for UNDO

        self.conditions = [self.new(Condition, reader)
            for i in range(numberOfConditions)]

        self.actions = [self.new(Action, reader)
            for i in range(numberOfActions)]

        reader.seek(currentPosition + size)

    def write(self, reader):
        newReader = ByteReader()

        newReader.writeByte(len(self.conditions), True)
        newReader.writeByte(len(self.actions), True)
        newReader.writeShort(self.flags.getFlags(), True)
        newReader.writeShort(self.is_restricted)
        newReader.writeShort(self.restrictCpt)
        newReader.writeShort(self.identifier)
        newReader.writeShort(self.undo)

        for condition in self.conditions:
            condition.write(newReader)

        for action in self.actions:
            action.write(newReader)

        reader.writeShort((len(newReader) + 2)*-1)
        reader.writeReader(newReader)

class Events(DataLoader):
    # cdef public:
    #     int maxObjects
    #     int maxObjectInfo
    #     int numberOfPlayers
    #     list qualifier_list
    #     dict qualifiers
    #     list numberOfConditions
    #     list items
    #     list groups

    def initialize(self):
        self.qualifiers = {}
        self.numberOfConditions = []
        self.items = []
        self.groups = []

    def read(self, reader):
        java = self.settings.get('java', False)
        while 1:
            identifier = reader.read(4)
            if identifier == HEADER:
                self.maxObjects = max(300, reader.readShort())
                self.maxObjectInfo = reader.readShort()
                self.numberOfPlayers = reader.readShort()
                self.numberOfConditions = [reader.readShort()
                    for _ in xrange(17)]
                self.qualifier_list = []
                self.qualifiers = qualifiers = {}
                for _ in xrange(reader.readShort()):
                    newQualifier = self.new(Qualifier, reader)
                    self.qualifier_list.append(newQualifier)
                    qualifiers[newQualifier.qualifier] = newQualifier

            elif identifier == EVENT_COUNT:
                # just ignoring here, we'll be getting
                # number of events in EVENTGROUP_DATA
                # anyway
                size = reader.readInt() # ignored
                if java:
                    if reader.read(4) == EVENTGROUP_DATA: # iPhone
                        java = False
                        reader.rewind(4)

            elif identifier == EVENTGROUP_DATA:
                size = reader.readInt()
                if java:
                    numberOfGroups = reader.readInt()
                    self.items.extend([self.new(EventGroup, reader)
                        for _ in xrange(numberOfGroups)])
                else:
                    endPosition = reader.tell() + size
                    while 1:
                        self.items.append(self.new(EventGroup, reader))
                        if reader.tell() >= endPosition:
                            break

            elif identifier == END:
                break
            else:
                import code
                code.interact(local = locals())
                raise NotImplementedError(
                    'identifier %r not implemented (%s)' % (identifier, reader.tell()))

    def write(self, reader):
        java = self.settings.get('java', False)

        eventReader = ByteReader()
        if java:
            eventReader.writeInt(len(self.items))
        for eventGroup in self.items:
            eventGroup.write(eventReader)

        reader.write(HEADER)
        reader.writeShort(self.maxObjects)
        reader.writeShort(self.maxObjectInfos)
        reader.writeShort(self.numberOfPlayers)
        for item in self.numberOfConditions:
            reader.writeShort(item)
        reader.writeShort(len(self.qualifiers))
        for item in self.qualifiers:
            item.write(reader)

        reader.write(EVENT_COUNT)
        reader.writeInt(len(eventReader))
        if java:
            reader.writeInt(len(self.items))

        if self.items:
            reader.write(EVENTGROUP_DATA)
            reader.writeInt(len(eventReader))
            reader.writeReader(eventReader)

        reader.write(END)
