from chowdren.writers.objects import ObjectWriter

from chowdren.common import get_animation_name, to_c, make_color

from chowdren.writers.events import (StaticConditionWriter,
    StaticActionWriter, StaticExpressionWriter, make_table,
    ActionWriter, ActionMethodWriter, ConditionMethodWriter,
    ExpressionMethodWriter, EmptyAction)

class SystemTray(ObjectWriter):
    class_name = 'SystemTray'
    static = True
    # filename = 'systemtray'

    def write_init(self, writer):
        pass

actions = make_table(ActionMethodWriter, {
    # systray_halt_system_commands_halt_close_command_15
    15 : EmptyAction,
})

actions.update(make_table(StaticActionWriter, {
}))

conditions = make_table(ConditionMethodWriter, {
})

expressions = make_table(ExpressionMethodWriter, {
})

def get_object():
    return SystemTray
