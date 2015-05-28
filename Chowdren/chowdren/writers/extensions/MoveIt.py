from chowdren.writers.objects import ObjectWriter
from mmfparser.bitdict import BitDict

from chowdren.common import get_animation_name, to_c, make_color

from chowdren.writers.events import (ComparisonWriter, ActionMethodWriter,
    ConditionMethodWriter, ExpressionMethodWriter, make_table)

class MoveIt(ObjectWriter):
    class_name = 'MoveIt'
    filename = 'moveit'

    def write_init(self, writer):
        pass

actions = make_table(ActionMethodWriter, {
})

conditions = make_table(ConditionMethodWriter, {
})

expressions = make_table(ExpressionMethodWriter, {
})

def get_object():
    return MoveIt
