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
    ConditionMethodWriter, ExpressionMethodWriter, make_table)
from mmfparser.data.font import LogFont
from mmfparser.bitdict import BitDict

EDIT_FLAGS = BitDict(
    'HorizontalScrollbar',
    'HorizontalAutoscroll',
    'VerticalScrollbar',
    'VerticalAutoscroll',
    'ReadOnly',
    'Multiline',
    'Password',
    'Border',
    'HideOnStart',
    'Uppercase',
    'Lowercase',
    'Tabstop',
    'SystemColor',
    '3DLook',
    'Transparent',
    None,
    'AlignCenter',
    'AlignRight'
)

class EditObject(ObjectWriter):
    class_name = 'EditObject'
    filename = 'editext'

    def write_init(self, writer):
        if not self.has_updates():
            return
        data = self.get_data()
        width = data.readShort(True)
        height = data.readShort(True)
        is_unicode = self.data.settings.get('unicode', False)
        font = self.data.new(LogFont, data, old=not is_unicode)
        data.skipBytes(4 * 16) # custom colors?
        foregroundColor = data.readColor()
        backgroundColor = data.readColor()
        data.skipBytes(40) # text-style?
        if is_unicode:
            data.skipBytes(40)
        flags = EDIT_FLAGS.copy()
        flags.setFlags(data.readInt())

        writer.putlnc('width = %s;', width)
        writer.putlnc('height = %s;', height)
        if flags['Password']:
            writer.putlnc('edit_flags |= PASSWORD;')
        if flags['Multiline']:
            writer.putlnc('edit_flags |= MULTILINE;')
        if flags['ReadOnly']:
            writer.putlnc('edit_flags |= READ_ONLY;')
        if flags['HideOnStart']:
            writer.putlnc('set_visible(false);')

        if self.converter.config.use_gwen():
            writer.putlnc('init_control();')

    @staticmethod
    def write_application(converter):
        if not converter.config.use_edit_obj():
            return
        converter.add_define('CHOWDREN_USE_EDITOBJ')

    def has_updates(self):
        return self.converter.config.use_edit_obj()

actions = make_table(ActionMethodWriter, {
    4 : 'set_text',
    13 : 'set_visible(false)',
    12 : 'set_visible(true)',
    16 : 'enable_focus',
    18 : 'disable',
    23 : 'set_limit',
    30 : 'disable_focus',
    33 : 'scroll_to_end'
})

conditions = make_table(ConditionMethodWriter, {
    4 : 'get_focus'
})

expressions = make_table(ExpressionMethodWriter, {
    0 : 'get_text()'
})

def get_object():
    return EditObject