from chowdren.writers.objects import ObjectWriter
from chowdren.common import get_animation_name, to_c, make_color
from chowdren.writers.events import (ComparisonWriter, ActionMethodWriter,
    ConditionMethodWriter, ExpressionMethodWriter, make_table)
from chowdren.stringhash import get_string_int_map

class SelectObject(ObjectWriter):
    class_name = 'SelectObject'
    static = True

    def write_init(self, writer):
        pass

class LowestValue(ConditionMethodWriter):
    custom = True

    def write(self, writer):
        obj = self.parameters[0].loader
        obj = (obj.objectInfo, obj.objectType)
        writer.start_brace()
        writer.putln('FrameObject * selected; selected = NULL;')
        writer.putlnc('int alt_index = %s;', self.convert_index(1))
        writer.putln('double lowest = 0.0;')
        writer.putln('double test_val;')
        with self.converter.iterate_object(obj, writer, copy=False):
            writer.putlnc('FrameObject * obj = %s;',
                          self.converter.get_object(obj))
            check = 'obj->alterables->values.get(alt_index)'
            writer.putlnc('test_val = %s;', check)
            writer.putlnc('if (selected == NULL || test_val < lowest) {')
            writer.indent()
            writer.putlnc('selected = obj;')
            writer.putlnc('lowest = test_val;')
            writer.end_brace()

        obj_list = self.converter.create_list(obj, writer)

        writer.putlnc('if (selected == NULL) %s', self.converter.event_break)
        writer.putlnc('%s.select_single(selected);', obj_list)
        writer.end_brace()


actions = make_table(ActionMethodWriter, {
})

conditions = make_table(ConditionMethodWriter, {
    2 : LowestValue
})

expressions = make_table(ExpressionMethodWriter, {
})

def get_object():
    return SelectObject
