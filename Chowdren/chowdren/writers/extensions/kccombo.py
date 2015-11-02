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

from mmfparser.bitdict import BitDict
from mmfparser.data.font import LogFont

COMBO_FLAGS = BitDict(
    'Simple',
    'Dropdown',
    'DropdownList',
    'ScrollBar',
    'Sort',
    'HideOnStart',
    'SystemColor',
    'Scroll',
    'OneBase'
)

class ComboBox(ObjectWriter):
    class_name = 'ComboBox'
    filename = 'combobox'

    def write_init(self, writer):
        data = self.get_data()
        width = data.readShort(True)
        height = data.readShort(True)
        is_unicode = self.data.settings.get('unicode', False)
        log_font = self.data.new(LogFont, data, old=not is_unicode)
        font_color = data.readColor()
        font_style = self.data.readString(data, 40)
        flags = COMBO_FLAGS.copy()
        flags.setFlags(data.readInt(True))
        line_count = data.readShort(True)
        bg_color = data.readColor()
        data.skipBytes(12)

        writer.putlnc('width = %s;', width)
        writer.putlnc('height = %s;', height)
        index_offset = -1 if flags['OneBase'] else 0
        writer.putlnc('index_offset = %s;', index_offset)
        # writer.putlnc('combo_box.m_BackgroundColor = Gwen::Color%s;', bg_color)
        writer.putlnc('Gwen::Color font_color%s;', font_color)
        # writer.putlnc('combo_box.SetTextColorOverride(font_color);')
        # writer.putlnc('combo_box.m_Menu->m_Color = font_color;');
        # writer.putlnc('combo_box.m_Menu->m_BackgroundColor = combo_box.m_BackgroundColor;');

        writer.putlnc('init_control();')

        for _ in xrange(line_count):
            line = self.data.readString(data)
            writer.putlnc('combo_box->AddItem(L"%s");', line)


    def has_updates(self):
        return True

class SelectionChanged(ConditionMethodWriter):
    is_always = True
    post_event = True
    method = 'is_selection_changed'

actions = make_table(ActionMethodWriter, {
    5 :  'reset',
    6 : 'add_line',
    9 : 'set_current_line',
    10 : 'set_visible(true)',
    11 : 'set_visible(false)',
    13 : 'highlight',
    14 : 'dehighlight',
    21 : 'lose_focus'
})

conditions = make_table(ConditionMethodWriter, {
    3 : SelectionChanged,
    5 : 'is_list_dropped',
})

expressions = make_table(ExpressionMethodWriter, {
    0 : 'get_current_line_number()',
    1 : 'get_current_line()',
    4 : 'get_line',
    16 : 'find_string_exact'
})

def get_object():
    return ComboBox
