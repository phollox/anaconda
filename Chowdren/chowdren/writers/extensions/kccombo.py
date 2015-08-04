from chowdren.writers.objects import ObjectWriter

from chowdren.common import get_animation_name, to_c, make_color

from chowdren.writers.events import (ComparisonWriter, ActionMethodWriter,
    ConditionMethodWriter, ExpressionMethodWriter, make_table)

from mmfparser.bitdict import BitDict
from mmfparser.data.font import LogFont

class ComboBox(ObjectWriter):
    class_name = 'ComboBox'
    filename = 'combobox'

    def write_init(self, writer):
        data = self.get_data()
        width = data.readShort(True)
        height = data.readShort(True)
        log_font = LogFont(data, old = True)
        font_color = data.readColor()
        font_style = data.readString(40)
        flags = data.readInt(True)
        line_count = data.readShort(True)
        bg_color = data.readColor()
        data.skipBytes(12)

        writer.putlnc('width = %s;', width)
        writer.putlnc('height = %s;', height)
        writer.putlnc('combo_box.m_BackgroundColor = Gwen::Color%s;', bg_color)
        writer.putlnc('Gwen::Color font_color%s;', font_color)
        writer.putlnc('combo_box.SetTextColorOverride(font_color);')
        writer.putlnc('combo_box.m_Menu->m_Color = font_color;');
        writer.putlnc('combo_box.m_Menu->m_BackgroundColor = combo_box.m_BackgroundColor;');

        for _ in xrange(0, line_count):
            line = data.readString()
            writer.putlnc('combo_box.AddItem(L"%s");', line)


    def has_updates(self):
        return True

actions = make_table(ActionMethodWriter, {
    5 :  'reset',
    6 : 'add_line',
    9 : 'set_current_line',
    11 : 'set_visible(false)',
    10 : 'set_visible(true)',
    14 : 'dehighlight',
    21 : 'lose_focus'
})

conditions = make_table(ConditionMethodWriter, {
    5 : 'is_list_dropped',
})

expressions = make_table(ExpressionMethodWriter, {
    0 : 'get_current_line_number',
    1 : 'get_current_line',
    4 : 'get_line',
    16 : 'find_string_exact'
})

def get_object():
    return ComboBox
