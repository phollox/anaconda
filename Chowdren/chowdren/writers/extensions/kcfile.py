# Copyright (c) Mathias Kaerlev 2012-2015.
#
# This file is part of Anaconda.
#
# Anaconda is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Anaconda is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Anaconda.  If not, see <http://www.gnu.org/licenses/>.

from chowdren.writers.objects import ObjectWriter

from chowdren.common import get_animation_name, to_c, make_color

from chowdren.writers.events import (StaticConditionWriter,
    StaticActionWriter, StaticExpressionWriter, make_table, EmptyAction,
    ActionMethodWriter, ExpressionMethodWriter, ConditionMethodWriter)

class File(ObjectWriter):
    class_name = 'File'
    static = True

    def write_init(self, writer):
        pass

class DialogAction(ActionMethodWriter):
    has_object = False

    def write(self, writer):
        converter = self.converter
        converter.extension_includes.add('objects/dialogext.h')
        converter.extension_sources.add('objects/dialogext.cpp')
        writer.put('DialogObject::')
        ActionMethodWriter.write(self, writer)

actions = make_table(StaticActionWriter, {
    0 : 'change_directory',
    2 : 'create_directory',
    5 : 'delete_file',
    6 : 'rename_file',
    8 : 'copy_file',
    10 : 'append_text',
    11 : EmptyAction
})

actions.update(make_table(DialogAction, {
    13 : 'set_title',
    16 : 'set_filter',
    17 : 'set_default_extension',
    18 : 'open_load_selector',
    19 : 'open_save_selector'
}))

class DialogCondition(ConditionMethodWriter):
    has_object = False
    is_always = True
    post_event = True

    def write(self, writer):
        converter = self.converter
        converter.extension_includes.add('objects/dialogext.h')
        converter.extension_sources.add('objects/dialogext.cpp')
        writer.put('DialogObject::')
        ConditionMethodWriter.write(self, writer)

conditions = make_table(StaticConditionWriter, {
    1 : 'name_exists',
    2 : 'file_readable',
    4 : 'file_exists',
    5 : 'directory_exists',
})

conditions.update(make_table(DialogCondition, {
    6 : 'is_file_success'    
}))

class DialogExpression(ExpressionMethodWriter):
    has_object = False

    def get_string(self):
        converter = self.converter
        converter.extension_includes.add('objects/dialogext.h')
        converter.extension_sources.add('objects/dialogext.cpp')
        return 'DialogObject::' + ExpressionMethodWriter.get_string(self)

expressions = make_table(StaticExpressionWriter, {
    0 : 'get_size',
    7 : 'get_ext',
    6 : 'get_title',
    27 : 'get_appdata_directory()', # directories_my_documents_directory
    28 : 'get_appdata_directory()'
})

expressions.update(make_table(DialogExpression, {
    15 : 'get_path()'
}))

def get_object():
    return File
