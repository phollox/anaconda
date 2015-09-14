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

    def has_updates(self):
        return self.converter.config.use_gwen()

actions = make_table(ActionMethodWriter, {
    2 : 'set_visible(false)',
    3 : 'enable', # enable
    4 : 'disable', # disable
    14 : 'check'
})

conditions = make_table(ConditionMethodWriter, {
})

expressions = make_table(ExpressionMethodWriter, {
})

def get_object():
    return ButtonObject