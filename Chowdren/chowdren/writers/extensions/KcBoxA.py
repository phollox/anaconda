from chowdren.writers.objects import ObjectWriter
from chowdren.common import (get_animation_name, to_c,
    make_color)
from chowdren.writers.events import (ComparisonWriter, ActionMethodWriter,
    ConditionMethodWriter, ExpressionMethodWriter, make_table, EmptyAction)
from mmfparser.data.font import LogFont
from mmfparser.bitdict import BitDict

def get_system_color(index):
    return None
    # if index == 0xFFFF:
    #     return None
    # if index & (1 << 31) != 0:
    #     return get_color_number(index)
    # try:
    #     return COLORS[index]
    # except KeyError:
    #     return (0, 0, 0)

def read_system_color(reader):
    return get_system_color(reader.readInt(True))

FLAGS = BitDict(
    'AlignTop',
    'AlignVerticalCenter',
    'AlignBottom',
    None,
    'AlignLeft',
    'AlignHorizontalCenter',
    'AlignRight',
    None,
    'Multiline',
    'NoPrefix',
    'EndEllipsis',
    'PathEllipsis',
    'Container',
    'Contained',
    'Hyperlink',
    None,
    'AlignImageTopLeft',
    'AlignImageCenter',
    'AlignImagePattern',
    None,
    'Button',
    'Checkbox',
    'ShowButtonBorder',
    'ImageCheckbox',
    'HideImage',
    'ForceClipping',
    None,
    None,
    'ButtonPressed',
    'ButtonHighlighted',
    'Disabled'
)

NONE, HYPERLINK, BUTTON, CHECKBOX = xrange(4)

class SystemBox(ObjectWriter):
    class_name = 'SystemBox'
    filename = 'systembox'
    use_alterables = True
    default_instance = 'default_systembox_instance'

    def write_init(self, writer):
        data = self.get_data()
        # data.skipBytes(4)

        width = data.readShort(True)
        height = data.readShort(True)
        writer.putlnc('width = %s;', width)
        writer.putlnc('height = %s;', height)

        flags = FLAGS.copy()
        flags.setFlags(data.readInt())
        self.show_border = flags['ShowButtonBorder']
        self.image_checkbox = flags['ImageCheckbox']
        if flags['Hyperlink']:
            display_type = HYPERLINK
        elif flags['Button']:
            if flags['Checkbox']:
                display_type = CHECKBOX
            else:
                display_type = BUTTON
        else:
            display_type = NONE
        align_top_left = flags['AlignImageTopLeft']
        align_center = flags['AlignImageCenter']
        pattern = flags['AlignImagePattern']
        fill = read_system_color(data)
        border1 = read_system_color(data)
        border2 = read_system_color(data)
        self.image = data.readShort()

        if self.image == -1:
            writer.putln('image = NULL;')
        else:
            writer.putln('image = %s;' % self.converter.get_image(self.image))
            if pattern:
                writer.putln('type = PATTERN_IMAGE;')
            elif align_center:
                writer.putln('type = CENTER_IMAGE;')
            elif align_top_left:
                writer.putln('type = TOPLEFT_IMAGE;')
            else:
                raise NotImplementedError()

        data.skipBytes(2) # rData_wFree
        text_color = read_system_color(data)
        margin_left = data.readShort()
        margin_top = data.readShort()
        margin_right = data.readShort()
        margin_bottom = data.readShort()

        font = LogFont(data, old=True)
        data.skipBytes(40) # file.readStringSize(40)
        data.adjust(8)
        text = data.readReader(data.readInt(True)).readString().rsplit('\\n',
                                                                       1)
        if len(text) == 1:
            text, = text
            tooltip = None
        else:
            text, tooltip = text
        new_width = width - margin_left - margin_right
        new_height = height - margin_top - margin_bottom

        writer.putlnc('text = std::string(%r, %s);', text, len(text))

        if flags['AlignTop']:
            y_align = 'top'
        elif flags['AlignVerticalCenter']:
            y_align = 'center'
        elif flags['AlignBottom']:
            y_align = 'bottom'

        if flags['AlignLeft']:
            x_align = 'left'
        elif flags['AlignHorizontalCenter']:
            x_align = 'center'
        elif flags['AlignRight']:
            x_align = 'right'

        version = data.readInt()
        hyperlink_color = read_system_color(data)

    def get_images(self):
        if self.image == -1:
            return ()
        return (self.image,)

actions = make_table(ActionMethodWriter, {
    0 : 'set_size',
    1 : 'set_global_position',
    6 : 'hide_fill',
    22 : 'hide_border_1',
    31 : 'hide_border_2',
    # actually system colors
    51 : 'set_fill(Color(%s))',
    52 : 'set_border_1(Color(%s))',
    53 : 'set_border_2(Color(%s))',
    54 : EmptyAction, # set text color, other
    55 : 'set_text',
    62 : 'set_visible(true)',
    63 : 'set_visible(false)'
})

conditions = make_table(ConditionMethodWriter, {
})

expressions = make_table(ExpressionMethodWriter, {
    29 : '.text',
    31 : '.width',
    32 : '.height',
    34 : 'get_x()',
    35 : 'get_y()'
})

def get_object():
    return SystemBox
