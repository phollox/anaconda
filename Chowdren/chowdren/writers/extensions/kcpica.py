from chowdren.writers.objects import ObjectWriter
from chowdren.common import get_animation_name, to_c, make_color
from chowdren.writers.events import (ComparisonWriter, ActionMethodWriter,
    ConditionMethodWriter, ExpressionMethodWriter, make_table)
from mmfparser.bitdict import BitDict

FLAGS = BitDict(
    'Resize',
    'HideOnStart',
    'TransparentBlack',
    'TransparentFirstPixel',
    'FlippedHorizontally',
    'FlippedVertically',
    'Resample',
    'WrapModeOff'
)

class ActivePicture(ObjectWriter):
    is_active_picture = True
    class_name = 'ActivePicture'
    filename = 'picture'
    use_alterables = True
    default_instance = 'default_picture_instance'

    def write_init(self, writer):
        data = self.get_data()
        if self.is_active_picture:
            width = data.readInt()
            height = data.readInt()
            flags_int = data.readInt(True)
            transparent_color = data.readColor()
            image = data.readString(260) or None
        else:
            width = data.readShort(True)
            height = data.readShort(True)
            flags_int = data.readShort(True)
            image = data.readString(260) or None
            transparent_color = data.readColor()

        flags = FLAGS.copy()
        flags.setFlags(flags_int)
        visible = not flags['HideOnStart']
        if not flags['TransparentFirstPixel']:
            writer.putln('set_transparent_color(%s);' %
                         make_color(transparent_color))
        writer.putlnc('sprite_col.width = width = %s;', width)
        writer.putlnc('sprite_col.height = height = %s;', height)
        if flags['Resize']:
            writer.putlnc('picture_flags |= FORCE_RESIZE;')
        # objects_file.putdef('filename', image)

actions = make_table(ActionMethodWriter, {
    0 : 'load',
    2 : 'set_hotspot',
    3 : 'set_size',
    11 : 'set_hotspot_mul(1.0f, 0.5f)',
    9 : 'set_hotspot_mul(0.0f, 0.5f)',
    10 : 'set_hotspot_mul(0.5f, 0.5f)',
    13 : 'set_hotspot_mul(0.5f, 1.0f)',
    21 : 'set_offset_y',
    25 : 'set_wrap(false)',
    26 : 'paste',
    15 : 'flip_horizontal',
    4 : 'set_angle',
    29 : 'set_zoom'
})

conditions = make_table(ConditionMethodWriter, {
    0 : '.image != NULL'
})

expressions = make_table(ExpressionMethodWriter, {
    0 : '.filename',
    1 : 'get_width()',
    2 : 'get_height()',
    3 : 'get_resized_width()',
    4 : 'get_resized_height()',
    9 : '.angle',
    13 : 'get_zoom_x()'
})

def get_object():
    return ActivePicture