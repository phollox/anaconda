from chowdren.writers.objects import ObjectWriter

from chowdren.common import get_animation_name, to_c, make_color

from chowdren.writers.events import (StaticConditionWriter,
    StaticActionWriter, StaticExpressionWriter, make_table, EmptyAction,
    ExpressionMethodWriter, TrueCondition)

class Yaso(ObjectWriter):
    class_name = 'Yaso'
    static = True

    def write_init(self, writer):
        pass

actions = make_table(StaticActionWriter, {
})

conditions = make_table(StaticConditionWriter, {
    1 : TrueCondition # is connected to the internet
})

expressions = make_table(ExpressionMethodWriter, {
    14 : '.empty_string',
    15 : 'platform_get_appdata_dir'
})

def get_object():
    return Yaso
