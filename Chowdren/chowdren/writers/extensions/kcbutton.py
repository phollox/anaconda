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

from chowdren.writers.objects import ObjectWriter
from chowdren.common import get_animation_name, to_c, make_color
from chowdren.writers.events import (ComparisonWriter, ActionMethodWriter,
    ConditionMethodWriter, ExpressionMethodWriter, make_table,
    EmptyAction)
from mmfparser.data.font import LogFont
from mmfparser.bitdict import BitDict

BUTTON_FLAGS = BitDict(
    'HideOnStart',
    'DisableOnStart',
    'TextOnLeft',
    'Transparent',
    'SystemColor'
)

PUSHTEXT_BUTTON = 0
CHECKBOX_BUTTON = 1
RADIO_BUTTON = 2
PUSHBITMAP_BUTTON = 3
PUSHTEXTBITMAP_BUTTON = 4

BUTTON_BORDER = 1

class ButtonObject(ObjectWriter):
    class_name = 'ButtonObject'
    filename = 'buttonext'

    def write_init(self, writer):
        if not self.has_updates():
            return
        flags = BUTTON_FLAGS.copy()
        data = self.get_data()
        width = data.readShort(True)
        height = data.readShort(True)
        writer.putlnc('width = %s;', width)
        writer.putlnc('height = %s;', height)
        button_type = data.readShort()
        button_count = data.readShort()
        button_flags = []
        if button_type == CHECKBOX_BUTTON:
            button_flags += ['IS_CHECKBOX']
        if button_flags:
            button_flags = ' | '.join(button_flags)
        else:
            button_flags = '0'
        writer.putlnc('init_button(%s);', button_flags)
        flags.setFlags(data.readInt())
        font = LogFont(data)
        font_color = data.readColor()
        back_color = data.readColor()
        if flags['SystemColor']:
            back_color = (255, 255, 255)
        images = [data.readShort() for _ in xrange(3)]
        data.readShort() # fourth word in img array
        data.readInt() # ebtnSecu
        align = data.readShort()
        tooltip = ''
        strings = []
        if button_type == RADIO_BUTTON:
            strings = [data.readString() for _ in xrange(button_count)]
        else:
            strings.append(data.readString())
            tooltip = data.readString()

        if flags['DisableOnStart']:
            writer.putlnc('disable();')

    def has_updates(self):
        return self.converter.config.use_gwen()

actions = make_table(ActionMethodWriter, {
    0 : 'set_text',
    1 : 'set_visible(true)',
    2 : 'set_visible(false)',
    3 : 'enable', # enable
    4 : 'disable', # disable
    14 : 'check',
    15 : 'uncheck'
})

class OnClick(ConditionMethodWriter):
    is_always = True
    post_event = True
    method = 'is_clicked'

conditions = make_table(ConditionMethodWriter, {
    1 : OnClick
})

expressions = make_table(ExpressionMethodWriter, {
})

def get_object():
    return ButtonObject