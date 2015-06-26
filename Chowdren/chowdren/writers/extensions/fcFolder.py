from chowdren.writers.objects import ObjectWriter

from chowdren.common import get_animation_name, to_c, make_color

from chowdren.writers.events import (StaticConditionWriter,
    StaticActionWriter, StaticExpressionWriter, make_table, EmptyAction,
    ExpressionMethodWriter)

class FolderObject(ObjectWriter):
    class_name = 'File'
    static = True

    def write_init(self, writer):
        pass

class StartWalk(StaticActionWriter):
    custom = True

    def write(self, writer):
        print 'Folder walk not implemented yet'

actions = make_table(StaticActionWriter, {
    1 : 'delete_folder',
    8 : StartWalk,
    15 : 'create_directory'
})

conditions = make_table(StaticConditionWriter, {
    15 : 'file_exists'
})

expressions = make_table(ExpressionMethodWriter, {
})

def get_object():
    return FolderObject
