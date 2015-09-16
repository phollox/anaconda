from chowdren.writers.objects import ObjectWriter

from chowdren.common import get_animation_name, to_c, make_color

from chowdren.writers.events import (ActionMethodWriter, ConditionMethodWriter,
    ExpressionMethodWriter, make_table, ActionWriter, EmptyAction)

class AdvancedMenu(ObjectWriter):
    class_name = 'AdvancedMenu'
    static = True

class Action(ActionMethodWriter):
    has_object = False

actions = make_table(Action, {
    2 : EmptyAction,
    6 : EmptyAction,
    8 : EmptyAction,
    20 : EmptyAction
})

conditions = make_table(ConditionMethodWriter, {
})

expressions = make_table(ExpressionMethodWriter, {
})

def get_object():
    return AdvancedMenu
