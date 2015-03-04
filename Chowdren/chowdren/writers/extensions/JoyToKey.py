from chowdren.writers.objects import ObjectWriter

from chowdren.common import get_animation_name, to_c, make_color

from chowdren.writers.events import (StaticConditionWriter,
    ActionWriter, ActionMethodWriter, StaticExpressionWriter, make_table,
    ConditionMethodWriter, ExpressionMethodWriter, EmptyAction, FalseCondition)

class JoyToKey(ObjectWriter):
    class_name = 'JoyToKey'
    static = True
    defines = ['CHOWDREN_USE_JOYTOKEY']

    def write_init(self, writer):
        pass

class AlwaysCondition(ConditionMethodWriter):
    is_always = True

actions = make_table(ActionMethodWriter, {
    0 : 'manager.map_button',
    1 : 'manager.map_axis',
    5 : 'manager.simulate_key',
    6 : EmptyAction, # detect devices
    7 : 'manager.set_deadzone'
})

conditions = make_table(AlwaysCondition, {
    5 : 'is_joystick_pressed_once(1, %s + 1)',
    6 : FalseCondition, # gamepad just connected
    7 : 'is_joystick_attached(1)',
    8 : FalseCondition,
    10 : '.manager.axis_moved',
    11 : 'any_joystick_pressed_once(1)'
})

expressions = make_table(ExpressionMethodWriter, {
    3 : 'get_joystick_axis(1, 1 + ',
    6 : 'get_joystick_name(1)',
    9 : '(get_joystick_last_press(1) - 1)',
    10 : '.manager.last_axis'
})

def get_object():
    return JoyToKey