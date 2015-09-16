from chowdren.writers.objects import ObjectWriter

from chowdren.common import get_animation_name, to_c, make_color

from chowdren.writers.events import (StaticConditionWriter, StaticActionWriter,
                                     StaticExpressionWriter, ExpressionWriter,
                                     make_table, EmptyAction)

class MessageBoxObject(ObjectWriter):
    class_name = 'DialogObject'
    filename = 'dialogext'
    static = True

    def write_init(self, writer):
        pass

actions = make_table(StaticActionWriter, {
    0 : 'set_title',
    1 : 'set_text',
    7 : 'set_ok',
    9 : 'set_yes_no',
    24 : 'set_modal',
    28 : 'set_topmost',
    35 : 'create'
})

conditions = make_table(StaticConditionWriter, {
})

expressions = make_table(StaticExpressionWriter, {
})

def get_object():
    return MessageBoxObject
