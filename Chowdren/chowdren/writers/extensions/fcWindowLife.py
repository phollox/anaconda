from chowdren.writers.objects import ObjectWriter

from chowdren.common import get_animation_name, to_c, make_color

from chowdren.writers.events import (ConditionMethodWriter,
    StaticConditionWriter, StaticActionWriter, StaticExpressionWriter,
    make_table, EmptyAction, ExpressionMethodWriter)

class WindowLife(ObjectWriter):
    class_name = 'File'
    static = True

    def write_frame(self, writer):
        pass

class OnResize(ConditionMethodWriter):
    is_always = True
    has_object = False
    custom = True

    def write(self, writer):
        writer.putlnc('static int old_width = -1;')
        writer.putlnc('static int old_height = -1;')
        writer.putlnc('int new_width, new_height;')
        writer.putlnc('platform_get_size(&new_width, &new_height);')
        writer.putlnc('if (old_width == new_width &&'
                      ' old_height == new_height) %s',
                      self.converter.event_break)
        writer.putlnc('old_width = new_width;')
        writer.putlnc('old_height = new_height;')

actions = make_table(StaticActionWriter, {
})

conditions = make_table(StaticConditionWriter, {
    3 : OnResize
})

expressions = make_table(ExpressionMethodWriter, {
})

def get_object():
    return WindowLife
