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

from mmfparser.bytereader import checkDefault
from mmfparser.loader import DataLoader
from mmfparser import byteflag
from mmfparser.bytereader import ByteReader
from mmfparser.bitdict import BitDict
from mmfparser.data.chunkloaders.movement import Movements
from mmfparser.data.chunkloaders.transition import FadeIn, FadeOut
from mmfparser.data.chunkloaders.objectinfo import (TEXT, QUESTION, SCORE,
    LIVES, COUNTER, RTF, SUBAPPLICATION)

class AlterableValues(DataLoader):
    # cdef public list items
    def read(self, reader):
        self.items = [reader.readInt()
            for _ in xrange(reader.readShort(True))]

    def write(self, reader):
        reader.writeShort(len(self.items))
        for item in self.items:
            reader.writeInt(item)

class AlterableStrings(DataLoader):
    # cdef public list items

    def initialize(self):
        self.items = []

    def read(self, reader):
        self.items = [self.readString(reader)
            for _ in xrange(reader.readShort(True))]

    def write(self, reader):
        reader.writeShort(len(self.items))
        for item in self.items:
            reader.writeString(item)

LINE_SHAPE = 1
RECTANGLE_SHAPE = 2
ELLIPSE_SHAPE = 3

SHAPE_TYPES = {
    1 : 'Line',
    2 : 'Rectangle',
    3 : 'Ellipse'
}

NONE_FILL = 0
SOLID_FILL = 1
GRADIENT_FILL = 2
MOTIF_FILL = 3

FILL_TYPES = [
    'None',
    'Solid',
    'Gradient',
    'Motif'
]

HORIZONTAL_GRADIENT = 0
VERTICAL_GRADIENT = 1

GRADIENT_TYPES = [
    'Horizontal',
    'Vertical'
]

SHAPE_FLAGS = BitDict(
    'InverseX',
    'InverseY'
)

class Shape(DataLoader):
    # cdef public:
    # int borderSize
    # int shape
    # int fillType
    # object lineFlags
    color1 = None
    color2 = None
    # int gradientFlags
    # int image

    def initialize(self):
        self.lineFlags = SHAPE_FLAGS.copy()

    def read(self, reader):
        self.borderSize = reader.readShort()
        self.borderColor = reader.readColor()
        self.shape = reader.readShort()
        self.fillType = reader.readShort()

        if self.shape == LINE_SHAPE:
            self.lineFlags.setFlags(reader.readShort(True))
        elif self.fillType == SOLID_FILL:
            self.color1 = reader.readColor()
        elif self.fillType == GRADIENT_FILL:
            self.color1 = reader.readColor()
            self.color2 = reader.readColor()
            self.gradientFlags = reader.readShort()
        elif self.fillType == MOTIF_FILL:
            self.image = reader.readShort()

    def write(self, reader):
        reader.writeShort(self.borderSize)
        reader.writeColor(self.borderColor)
        reader.writeShort(self.shape)
        reader.writeShort(self.fillType)
        if self.shape == LINE_SHAPE:
            reader.writeShort(self.lineFlags.getFlags())
        elif self.fillType == SOLID_FILL:
            reader.writeColor(self.color1)
        elif self.fillType == GRADIENT_FILL:
            reader.writeColor(self.color1)
            reader.writeColor(self.color2)
            reader.writeShort(self.gradientFlags)
        elif self.fillType == MOTIF_FILL:
            reader.writeShort(self.image)

    def getImage(self, images):
        return images.fromHandle(self.image)

    def getGradientType(self):
        return GRADIENT_TYPES[self.gradientFlags]

    def getFill(self):
        return FILL_TYPES[self.fillType]

    def getShape(self):
        return SHAPE_TYPES[self.shape]

NONE_OBSTACLE = 0
SOLID_OBSTACLE = 1
PLATFORM_OBSTACLE = 2
LADDER_OBSTACLE = 3
TRANSPARENT_OBSTACLE = 4

OBSTACLE_TYPES = [
    'None',
    'Solid',
    'Platform',
    'Ladder',
    'Transparent'
]

FINE_COLLISION = 0
BOX_COLLISION = 1

COLLISION_MODES = [
    'Fine',
    'Box'
]

class _Background(DataLoader):
    def getCollisionMode(self):
        return COLLISION_MODES[self.collisionMode]

    def getObstacleType(self):
        return OBSTACLE_TYPES[self.obstacleType]

    def getImage(self, imageBank):
        return imageBank.fromHandle(self.image)

    def isBackground(self):
        return True

class QuickBackdrop(_Background):
    # cdef public:
    #     int obstacleType
    #     int collisionMode
    #     int width
    #     int height
    #     object shape

    def read(self, reader):
        size = reader.readInt(True)
        self.obstacleType = reader.readShort()
        self.collisionMode = reader.readShort()
        self.width = reader.readInt()
        self.height = reader.readInt()
        self.shape = self.new(Shape, reader, width = self.width,
            height = self.height)

    def write(self, reader):
        reader.writeInt(0) # size
        reader.writeShort(self.obstacleType)
        reader.writeShort(self.collisionMode)
        reader.writeInt(self.width)
        reader.writeInt(self.height)
        self.shape.write(reader)

class Backdrop(_Background):
    # cdef public:
    #     int obstacleType
    #     int collisionMode
    #     int width
    #     int height
    #     int image

    def read(self, reader):
        size = reader.readInt()
        self.obstacleType = reader.readShort()
        self.collisionMode = reader.readShort()
        self.width = reader.readInt()
        self.height = reader.readInt()
        self.image = reader.readShort()

    def write(self, reader):
        reader.writeInt(0) # size
        reader.writeShort(self.obstacleType)
        reader.writeShort(self.collisionMode)
        reader.writeInt(self.width)
        reader.writeInt(self.height)
        reader.writeShort(self.image)

    def getImage(self, imageBank):
        return imageBank.fromHandle(self.image)

HAS_SINGLE_SPEED = [
    0, 3, 4, 6
]

class AnimationDirection(DataLoader):
    # cdef public:
    #     list frames
    #     int minSpeed
    #     int maxSpeed
    #     bint hasSingle
    #     int repeat
    #     int backTo

    def initialize(self):
        self.frames = []

    def read(self, reader):
        self.minSpeed = reader.readByte(True)
        self.maxSpeed = reader.readByte(True)
        if self.settings['index'] in HAS_SINGLE_SPEED:
            self.minSpeed = self.maxSpeed
            self.hasSingle = True
        self.repeat = reader.readShort()
        self.backTo = reader.readShort()
        self.frames = [reader.readShort()
            for _ in xrange(reader.readShort())]

    def getImages(self, images):
        return [images.fromHandle(frame) for frame in self.frames]

    def write(self, reader):
        reader.writeByte(self.minSpeed)
        reader.writeByte(self.maxSpeed)
        reader.writeShort(self.repeat)
        reader.writeShort(self.backTo)
        reader.writeShort(len(self.frames))
        for item in self.frames:
            reader.writeShort(item)

def getClosestDirection(direction, directionDict):
    try:
        return directionDict[direction]
    except KeyError:
        pass

    # (directionObject, distance)
    forward = None
    backward = None

    # get closest in back
    position = direction
    distance = 0
    while 1:
        position -= 1
        distance += 1
        if position < 0:
            position = 31
        if position in directionDict:
            backward = (directionDict[position], distance)
            break

    # get closest ahead
    position = direction
    distance = 0
    while 1:
        position = (position + 1) % 32
        distance += 1
        if position in directionDict:
            forward = (directionDict[position], distance)
            break

    # backward has priority
    if backward[1] >= forward[1]:
        return forward[0]
    else:
        return backward[0]

ANIMATION_NAMES = [
    'Stopped',
    'Walking',
    'Running',
    'Appearing',
    'Disappearing',
    'Bouncing',
    'Shooting',
    'Jumping',
    'Falling',
    'Climbing',
    'Crouch down',
    'Stand up',
    'User defined 1',
    'User defined 2',
    'User defined 3',
    'User defined 4'
]

class Animation(DataLoader):
    # cdef public:
    #     index
    #     cdef list directions
    #     cdef dict loadedDirections

    def initialize(self):
        self.directions = []
        self.loadedDirections = {}

    def read(self, reader):
        self.index = self.settings['index']
        currentPosition = reader.tell()

        offsets = [reader.readShort() for _ in xrange(32)]

        directionDict = self.loadedDirections = {}
        for index, offset in enumerate(offsets):
            if offset != 0:
                reader.seek(currentPosition + offset)
                directionDict[index] = self.new(AnimationDirection, reader)

        for index in xrange(32):
            self.directions.append(getClosestDirection(index,
                directionDict))

    def write(self, reader):
        directionData = ByteReader()
        for i in xrange(32):
            if i not in self.loadedDirections:
                reader.writeShort(0)
                continue
            reader.writeShort(directionData.tell() + 2 * 32)
            self.loadedDirections[i].write(directionData)
        reader.writeReader(directionData)

    def getIndex(self):
        return self.index

    def getName(self):
        index = self.getIndex()
        try:
            return ANIMATION_NAMES[index]
        except:
            return 'User defined %s' % (index - 12 + 1)

STOPPED = 0
WALKING = 1
RUNNING = 2
APPEARING = 3
DISAPPEARING = 4
BOUNCING = 5
SHOOTING = 6
JUMPING = 7
FALLING = 8
CLIMBING = 9
CROUCH_DOWN = 10
STAND_UP = 11

ANIMATION_ALTERNATIVES = {
    STOPPED : [APPEARING, WALKING, RUNNING],
    WALKING : [RUNNING],
    RUNNING : [WALKING],
    APPEARING : [WALKING, RUNNING],
    BOUNCING : [WALKING, RUNNING],
    SHOOTING : [WALKING, RUNNING],
    JUMPING : [WALKING, RUNNING],
    FALLING : [WALKING, RUNNING],
    CLIMBING : [WALKING, RUNNING],
    CROUCH_DOWN : [WALKING, RUNNING],
    STAND_UP : [WALKING, RUNNING]
}

def getClosestAnimation(index, animationDict, count):
    try:
        return animationDict[index]
    except KeyError:
        pass
    try:
        for alternative in ANIMATION_ALTERNATIVES[index]:
            if alternative in animationDict:
                return animationDict[alternative]
    except KeyError:
        pass
    for i in xrange(count):
        if i in animationDict:
            return animationDict[i]
    raise IndexError('no animation could be found for %r' % (
        ANIMATION_NAMES[index]))

class AnimationHeader(DataLoader):
    # cdef public:
    #     dict loadedAnimations
    #     list items

    def initialize(self):
        self.loadedAnimations = {}
        self.items = []

    def read(self, reader):
        currentPosition = reader.tell()

        size = reader.readShort()

        count = reader.readShort()

        offsets = [reader.readShort() for _ in xrange(count)]

        self.loadedAnimations = animationDict = {}
        for index, offset in enumerate(offsets):
            if offset != 0:
                reader.seek(currentPosition + offset)
                animationDict[index] = self.new(Animation, reader,
                    index = index)

        for index in xrange(count):
            self.items.append(getClosestAnimation(index, animationDict, count))

    def write(self, reader):
        count = max(16, max(self.loadedAnimations.keys()) + 1)
        offsets = []
        animationData = ByteReader()
        for i in xrange(count):
            if i not in self.loadedAnimations:
                offsets.append(0)
                continue
            offsets.append(animationData.tell() + 4 + 2 * count)
            self.loadedAnimations[i].write(animationData)
        reader.writeShort(len(animationData) + 4 + 2 * count) # size
        reader.writeShort(count)
        for offset in offsets:
            reader.writeShort(offset)
        reader.writeReader(animationData)

    def fromName(self, name):
        index = ANIMATION_NAMES.index(name)
        return self.items[index]

HIDDEN = 0
NUMBERS = 1
VERTICAL_BAR = 2
HORIZONTAL_BAR = 3
ANIMATION = 4
TEXT_COUNTER = 5

DISPLAY_NAMES = [
    'Hidden',
    'Numbers',
    'VerticalBar',
    'HorizontalBar',
    'Animation',
    'Text'
]

COUNTER_FRAMES = [
    '0',
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    '-',
    '+',
    '.',
    'e'
]

class InvalidFont(object):
    handle = None
    checksum = None
    references = None
    value = None

INVALID_FONT = InvalidFont()

INT_DIGITS_MASK = 0xF
FLOAT_DIGITS_MASK = 0xF0
FORMAT_FLOAT = 0x0200
FLOAT_DIGITS_SHIFT = 4
USE_DECIMALS = 0x0400
FLOAT_DECIMALS_MASK = 0xF000
FLOAT_DECIMALS_SHIFT = 12
FLOAT_PAD = 0x0800

class Counters(DataLoader):
    # cdef public:
    #     int width
    #     int height
    #     int player
    #     int displayType
    #     int flags
    #     bint inverse
    #     int font
    #     list frames
    #     object shape
    #     int integerDigits
    #     bint formatFloat
    #     int formatFloats
    #     int floatDigits
    #     bint useDecimals
    #     int decimals
    #     bint addNulls

    def read(self, reader):
        size = reader.readInt(True)
        self.width = reader.readInt(True)
        self.height = reader.readInt(True)
        self.player = reader.readShort(True)
        self.displayType = reader.readShort()
        self.flags = reader.readShort()

        self.integerDigits = self.flags & INT_DIGITS_MASK
        self.formatFloat = self.flags & FORMAT_FLOAT != 0
        self.floatDigits = ((self.flags & FLOAT_DIGITS_MASK
            ) >> FLOAT_DIGITS_SHIFT) + 1
        self.useDecimals = self.flags & USE_DECIMALS != 0
        self.decimals = ((self.flags & FLOAT_DECIMALS_MASK
            ) >> FLOAT_DECIMALS_SHIFT)
        self.addNulls = self.flags & FLOAT_PAD != 0

        self.inverse = byteflag.getFlag(self.flags, 8)
        self.font = reader.readShort(True)
        if self.displayType == HIDDEN:
            pass
        elif self.displayType in (NUMBERS, ANIMATION):
            self.frames = [reader.readShort()
                for _ in xrange(reader.readShort(True))]
        elif self.displayType in (VERTICAL_BAR, HORIZONTAL_BAR, TEXT_COUNTER):
            self.shape = self.new(Shape, reader)

    def write(self, reader):
        reader.writeInt(0) # size
        reader.writeInt(self.width, True)
        reader.writeInt(self.height, True)
        reader.writeShort(self.displayType)
        reader.writeShort(self.flags)
        reader.writeShort(self.font, True)
        if self.displayType in (NUMBERS, ANIMATION):
            reader.writeInt(len(self.frames), True)
            for frame in self.frames:
                reader.writeShort(frame)
        elif self.displayType in (VERTICAL_BAR, HORIZONTAL_BAR, TEXT_COUNTER):
            self.shape.write(reader)

    def getFont(self, fonts):
        try:
            return fonts.fromHandle(self.font)
        except (ValueError, AttributeError):
            return INVALID_FONT

    def getImage(self, name, images):
        return images.fromHandle(self.frames[COUNTER_FRAMES.index(name)])

    def getDisplayType(self):
        return DISPLAY_NAMES[self.displayType]

PARAGRAPH_FLAGS = BitDict(
    'HorizontalCenter',
    'RightAligned',
    'VerticalCenter',
    'BottomAligned',
    None, None, None, None,
    'Correct',
    'Relief'
)

class Paragraph(DataLoader):
    # cdef public:
    #     int font
    #     object flags
    #     object color
    #     bytes value

    def initialize(self):
        self.flags = PARAGRAPH_FLAGS.copy()

    def read(self, reader):
        self.font = reader.readShort(True)
        self.flags.setFlags(reader.readShort(True))
        self.color = reader.readColor()
        self.value = self.readString(reader)

    def write(self, reader):
        reader.writeShort(self.font, True)
        reader.writeShort(self.flags.getFlags(), True)
        reader.writeColor(self.color)
        reader.writeString(self.value)

    def getFont(self, fonts):
        try:
            return fonts.fromHandle(self.font)
        except (ValueError, AttributeError):
            return INVALID_FONT

class Text(DataLoader):
    # cdef public:
    #     int width
    #     int height
    #     list items

    def initialize(self):
        self.items = []

    def read(self, reader):
        currentPosition = reader.tell()
        size = reader.readInt()
        self.width = reader.readInt()
        self.height = reader.readInt()

        itemOffsets = [reader.readInt()
            for _ in xrange(reader.readInt())]

        for offset in itemOffsets:
            reader.seek(currentPosition + offset)
            self.items.append(self.new(Paragraph, reader))

    def write(self, reader):
        count = len(self.items)
        offsets = []
        paragraphData = ByteReader()
        for item in self.items:
            offsets.append(paragraphData.tell() + 16 + 4 * count)
            item.write(paragraphData)

        reader.writeInt(len(paragraphData) + 16 + 4 * count)
        reader.writeInt(self.width)
        reader.writeInt(self.height)
        reader.writeInt(count)
        for offset in offsets:
            reader.writeInt(offset)
        reader.writeReader(paragraphData)

RTF_FLAGS = BitDict(
    'Transparent',
    'VerticalSlider',
    'HorizontalSlider'
)

class RTFObject(DataLoader):
    # cdef public:
    #     int version
    #     object options
    #     tuple backColor
    #     int width
    #     int height
    #     bytes value

    def initialize(self):
        self.options = RTF_FLAGS.copy()

    def read(self, reader):
        size = reader.readInt()
        self.version = reader.readInt()
        self.options.setFlags(reader.readInt())
        self.backColor = reader.readColor()
        self.width = reader.readInt()
        self.height = reader.readInt()
        reader.skipBytes(4)
        self.value = reader.read(reader.readInt())

    def write(self, reader):
        reader.writeInt(0)
        reader.writeInt(self.version)
        reader.writeInt(self.options)
        reader.writeColor(self.backColor)
        reader.writeInt(self.width)
        reader.writeInt(self.height)
        reader.writeInt(0)
        reader.writeInt(len(self.value))
        reader.write(self.value)

DOCK_POSITIONS = {
    (False, False) : 'Left',
    (True, False) : 'Top',
    (False, True) : 'Right',
    (True, True) : 'Bottom'
}

SUBAPPLICATION_FLAGS = BitDict(
    'ShareGlobals',
    'ShareLives',
    'ShareScores',
    None, #'SHARE_WINATTRIB',
    'Stretch',
    'Popup',
    'Caption',
    'ToolCaption',
    'Border',
    'Resizable',
    'SystemMenu',
    'DisableClose',
    'Modal',
    'DialogFrame',
    'Internal',#'INTERNAL',
    'HideOnClose',
    'CustomableSize',
    None,#'INTERNALABOUTBOX',
    'ClipSiblings',
    'SharePlayerControls',
    'MDIChild',
    'Docked',
    'Docked1',
    'Docked2',
    'Reopen',
    'RunEvenIfNotActive'
)

class SubApplication(DataLoader):
    # cdef public:
    #     int width
    #     int height
    #     int version
    #     int startFrame
    #     object options
    #     int iconOffset
    #     bytes name

    def initialize(self):
        self.options = SUBAPPLICATION_FLAGS.copy()

    def read(self, reader):
        size = reader.readInt()
        self.width = reader.readInt()
        self.height = reader.readInt()
        self.version = reader.readShort()
        self.startFrame = reader.readShort()
        self.options.setFlags(reader.readInt(True))
        self.iconOffset = reader.readInt()
        reader.skipBytes(4) # "free"
        self.name = self.readString(reader)

    def getDockedPosition(self):
        docked1 = self.options['Docked1']
        docked2 = self.options['Docked2']
        return DOCK_POSITIONS[(docked1, docked2)]

    def write(self, reader):
        reader.writeInt(0)
        reader.writeInt(self.width)
        reader.writeInt(self.height)
        reader.writeShort(self.version)
        reader.writeShort(self.startFrame)
        reader.writeInt(self.options.getFlags(), True)
        reader.writeInt(self.iconOffset)
        reader.writeInt(0)
        reader.writeString(self.name)

class Counter(DataLoader):
    # cdef public:
    #     int initial
    #     int minimum
    #     int maximum

    def read(self, reader):
        size = reader.readShort()
        self.initial = reader.readInt()
        self.minimum = reader.readInt()
        self.maximum = reader.readInt()

    def write(self, reader):
        reader.writeShort(0)
        reader.writeInt(self.initial)
        reader.writeInt(self.minimum)
        reader.writeInt(self.maximum)

# free(Villy)
# everything's good now! thanks Villy!

NEW_OBJECT_FLAGS = BitDict(
    'DoNotSaveBackground',
    'SolidBackground',
    'CollisionBox',
    'VisibleAtStart',
    'ObstacleSolid',
    'ObstaclePlatform',
    'AutomaticRotation'
)

OBJECT_FLAGS = BitDict(
    'DisplayInFront',
    'Background',
    'Backsave',
    'RunBeforeFadeIn',
    'Movements',
    'Animations',
    'TabStop',
    'WindowProc',
    'Values',
    'Sprites',
    'InternalBacksave',
    'ScrollingIndependant',
    'QuickDisplay',
    'NeverKill',
    'NeverSleep',
    'ManualSleep',
    'Text',
    'DoNotCreateAtStart',
    'FakeSprite',
    'FakeCollisions'
)

OBJECT_PREFERENCES = BitDict(
    'Backsave',
    'ScrollingIndependant',
    'QuickDisplay',
    'Sleep',
    'LoadOnCall',
    'Global',
    'BackEffects',
    'Kill',
    'InkEffects',
    'Transitions',
    'FineCollisions',
    'AppletProblems'
)

class ObjectCommon(DataLoader):
    newFlags = None
    flags = None
    preferences = None
    version = 0
    movements = None
    animations = None
    qualifiers = None
    identifier = 0
    backColour = None
    values = None
    strings = None
    text = None
    subApplication = None
    rtf = None
    counters = None
    counter = None
    extensionVersion = None
    extensionId = None
    extensionPrivate = None
    extensionData = None
    fadeIn = None
    fadeOut = None

    def initialize(self):
        self.qualifiers = []
        # OCFLAGS2
        self.newFlags = NEW_OBJECT_FLAGS.copy()
        # OEFLAG
        self.flags = OBJECT_FLAGS.copy()
        # OEPREF
        self.preferences = OBJECT_PREFERENCES.copy()

    def read(self, reader):
        currentPosition = reader.tell()

        size = reader.readInt()

        # cdef short movementsOffset
        # cdef short animationsOffset
        # cdef short counterOffset
        # cdef short systemObjectOffset
        # fadeInOffset
        # fadeOutOffset
        # cdef short valuesOffset
        # cdef short stringsOffset
        # cdef short extensionOffset
        # cdef short qualifier
        # cdef unsigned int end

        newobj = (self.settings['build'] >= 284 and
                            not self.settings.get('compat', False))
        newobj2 = True

        if newobj and newobj2:
            animationsOffset = reader.readShort()
            movementsOffset = reader.readShort()
            self.version = reader.readShort()
            reader.skipBytes(2) # "free"
            extensionOffset = reader.readShort()
            counterOffset = reader.readShort()
            self.flags.setFlags(reader.readInt(True))

            end = reader.tell() + 8 * 2

            for _ in xrange(8):
                qualifier = reader.readShort()
                if qualifier == -1:
                    break
                self.qualifiers.append(qualifier)

            reader.seek(end)

            systemObjectOffset = reader.readShort()

            valuesOffset = reader.readShort()
            stringsOffset = reader.readShort()
            self.newFlags.setFlags(reader.readShort(True))
            self.preferences.setFlags(reader.readShort(True)) # runtime data
            self.identifier = reader.readInt()
            self.backColour = reader.readColor()
            fadeInOffset = reader.readInt()
            fadeOutOffset = reader.readInt()
        elif newobj:
            counterOffset = reader.readShort()
            self.version = reader.readShort()
            reader.skipBytes(2) # "free"
            movementsOffset = reader.readShort()
            extensionOffset = reader.readShort()
            animationsOffset = reader.readShort()
            self.flags.setFlags(reader.readInt(True))

            end = reader.tell() + 8 * 2

            for _ in xrange(8):
                qualifier = reader.readShort()
                if qualifier == -1:
                    break
                self.qualifiers.append(qualifier)

            reader.seek(end)

            systemObjectOffset = reader.readShort()

            valuesOffset = reader.readShort()
            stringsOffset = reader.readShort()
            self.newFlags.setFlags(reader.readShort(True))
            self.preferences.setFlags(reader.readShort(True)) # runtime data
            self.identifier = reader.readInt()
            self.backColour = reader.readColor()
            fadeInOffset = reader.readInt()
            fadeOutOffset = reader.readInt()
        else:
            # start change
            movementsOffset = reader.readShort()
            animationsOffset = reader.readShort()
            self.version = reader.readShort()
            counterOffset = reader.readShort()
            systemObjectOffset = reader.readShort()
            reader.skipBytes(2) # "free"
            # stop change

            self.flags.setFlags(reader.readInt(True))

            end = reader.tell() + 8 * 2

            for _ in xrange(8):
                qualifier = reader.readShort()
                if qualifier == -1:
                    break
                self.qualifiers.append(qualifier)

            reader.seek(end)

            # can change
            extensionOffset = reader.readShort()

            valuesOffset = reader.readShort()
            stringsOffset = reader.readShort()
            self.newFlags.setFlags(reader.readShort(True))
            self.preferences.setFlags(reader.readShort(True)) # runtime data
            self.identifier = reader.readInt()
            self.backColour = reader.readColor()
            fadeInOffset = reader.readInt()
            fadeOutOffset = reader.readInt()

        if movementsOffset != 0:
            reader.seek(currentPosition + movementsOffset)
            self.movements = self.new(Movements, reader)

        if valuesOffset != 0:
            reader.seek(currentPosition + valuesOffset)
            self.values = self.new(AlterableValues, reader)

        if stringsOffset != 0:
            reader.seek(currentPosition + stringsOffset)
            self.strings = self.new(AlterableStrings, reader)

        if animationsOffset != 0:
            reader.seek(currentPosition + animationsOffset)
            self.animations = self.new(AnimationHeader, reader)

        if counterOffset != 0:
            reader.seek(currentPosition + counterOffset)
            self.counter = self.new(Counter, reader)

        if extensionOffset != 0:
            reader.seek(currentPosition + extensionOffset)

            dataSize = reader.readInt() - 20
            reader.skipBytes(4) # maxSize
            self.extensionVersion = reader.readInt()
            self.extensionId = reader.readInt()
            self.extensionPrivate = reader.readInt()
            if dataSize != 0:
                self.extensionData = reader.read(dataSize)

        if fadeInOffset != 0:
            reader.seek(currentPosition + fadeInOffset)
            self.fadeIn = self.new(FadeIn, reader)

        if fadeOutOffset != 0:
            reader.seek(currentPosition + fadeOutOffset)
            self.fadeOut = self.new(FadeOut, reader)

        if systemObjectOffset != 0:
            reader.seek(currentPosition + systemObjectOffset)

            objectType = self.parent.objectType
            if objectType in (TEXT, QUESTION):
                self.text = self.new(Text, reader)
            elif objectType in (SCORE, LIVES, COUNTER):
                self.counters = self.new(Counters, reader)
            elif objectType == RTF:
                self.rtf = self.new(RTFObject, reader)
            elif objectType == SUBAPPLICATION:
                self.subApplication = self.new(SubApplication, reader)

    def write(self, reader):
        movementsOffset = animationsOffset = counterOffset = \
        systemObjectOffset = extensionOffset = valuesOffset = \
        stringsOffset = fadeInOffset = fadeOutOffset = 0

        objectData = ByteReader()
        if self.movements is not None:
            movementsOffset = objectData.tell() + 62
            self.movements.write(objectData)
        if self.values is not None:
            valuesOffset = objectData.tell() + 62
            self.values.write(objectData)
        if self.strings is not None:
            stringsOffset = objectData.tell() + 62
            self.strings.write(objectData)
        if self.animations is not None:
            animationsOffset = objectData.tell() + 62
            self.animations.write(objectData)
        if self.counter is not None:
            counterOffset = objectData.tell() + 62
            self.counter.write(objectData)
        if self.extensionVersion is not None:
            extensionOffset = objectData.tell() + 62
            objectData.writeInt(20 + len(self.extensionData or ''))
            objectData.writeInt(20 + len(self.extensionData or ''))
            objectData.writeInt(self.extensionVersion)
            objectData.writeInt(self.extensionId)
            objectData.writeInt(self.extensionPrivate)
            objectData.write(self.extensionData or b'')
        if self.fadeIn is not None:
            fadeInOffset = objectData.tell() + 62
            self.fadeIn.write(objectData)
        if self.fadeOut is not None:
            fadeOutOffset = objectData.tell() + 62
            self.fadeOut.write(objectData)
        objectType = self.parent.objectType
        if objectType in (TEXT, QUESTION, SCORE, LIVES, COUNTER, RTF,
                        SUBAPPLICATION):
            systemObjectOffset = objectData.tell() + 62
            if objectType in (TEXT, QUESTION):
                self.text.write(objectData)
            elif objectType in (SCORE, LIVES, COUNTER):
                self.counters.write(objectData)
            elif objectType == RTF:
                self.rtf.write(objectData)
            elif objectType == SUBAPPLICATION:
                self.subApplication.write(objectData)

        reader.writeInt(len(objectData) + 62) # size
        reader.writeShort(movementsOffset)
        reader.writeShort(animationsOffset)
        reader.writeShort(self.version)
        reader.writeShort(counterOffset)
        reader.writeShort(systemObjectOffset)
        reader.writeShort(0)
        reader.writeInt(self.flags.getFlags(), True)

        for i in xrange(8):
            try:
                reader.writeShort(self.qualifiers[i])
            except IndexError:
                reader.writeShort(-1)
        reader.writeShort(extensionOffset)
        reader.writeShort(valuesOffset)
        reader.writeShort(stringsOffset)
        reader.writeShort(self.newFlags.getFlags(), True)
        reader.writeShort(self.preferences.getFlags(), True)
        reader.writeInt(self.identifier)
        reader.writeColor(self.backColour)
        reader.writeInt(fadeInOffset)
        reader.writeInt(fadeOutOffset)
        reader.writeReader(objectData)

    def isBackground(self):
        return self.flags['QuickDisplay'] or self.flags['Background']

__all__ = ['AlterableValues', 'AlterableStrings', 'Backdrop', 'QuickBackdrop',
    'AnimationDirection', 'Animation', 'AnimationHeader', 'ObjectCommon']