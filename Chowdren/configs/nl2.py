from chowdren.writers.events.system import get_loop_index_name
from chowdren.common import is_qualifier
import re
import os

def use_deferred_collisions(converter):
    return False

def init(converter):
    converter.add_define('CHOWDREN_IS_NL2')
    converter.add_define('CHOWDREN_QUICK_SCALE')
    converter.add_define('CHOWDREN_POINT_FILTER')
    converter.add_define('CHOWDREN_3DS_BORDERS')
    converter.add_define('CHOWDREN_OBSTACLE_IMAGE')
    converter.add_define('CHOWDREN_TEXTURE_GC')
    converter.add_define('CHOWDREN_DEFAULT_SCALE', 2)
    converter.add_define('CHOWDREN_BLITTER_EXT_CALLBACKS')
    converter.add_define('CHOWDREN_IGNORE_INACTIVE')
    converter.add_define('CHOWDREN_NO_PASTE')
    converter.add_define('CHOWDREN_NL2_CONSOLE')

def init_group(converter, group):
    if len(group.conditions) != 1:
        return
    if group.conditions[0].data.getName() != 'CompareAlterableValue':
        return
    if len(group.actions) != 2:
        return
    action = group.actions[0]
    if action.data.getName() != 'SetAlterableValue':
        return
    obj = action.get_object()
    if is_qualifier(obj[0]):
        return
    writer = converter.get_object_writer(obj)
    if writer.data.name != 'Reserve Values':
        return
    param = action.parameters[0]
    if param.getName() != 'AlterableValue':
        return
    if param.loader.value not in (12, 14):
        return
    print 'Removing alterable set for', param.loader.value
    group.actions = group.actions[1:]

def finish(converter):
    obj = converter.name_to_writer['Reserve Values'][0]
    obj_list = converter.object_list_ids[obj.get_list_id()][0]
    converter.add_define('nl2_values_list', obj_list)
    obj = converter.name_to_writer['Top-Lefter'][0]
    obj_list = converter.object_list_ids[obj.get_list_id()][0]
    converter.add_define('nl2_topleft_list', obj_list)

def use_image_preload(converter):
    return True

def use_image_flush(converter, frame):
    return True

alterable_int_objects = [
]

global_objects = [
    'Score!',
    'Time!',
    'Hit Count',
    'Enemy Count',
    'Top Combo Level',
    'Player Mask',
    'Reserve Values'
]

def use_transparency_shader_reset(converter):
    return True

def use_global_alterables(converter, obj):
    for name in global_objects:
        if obj.data.name == name:
            return True
    return False

def use_single_global_alterables(converter, obj):
    return True

def use_alterable_int(converter, expression):
    obj = expression.get_object()
    name = expression.converter.get_object_name(obj)
    for (check_name, alts) in alterable_int_objects:
        if not name.startswith(check_name):
            continue
        if alts is None:
            return True
        index = expression.data.loader.value
        return index in alts
    return False

def use_repeated_collisions(converter):
    return False

def get_string(converter, value):
    if converter.platform_name == '3ds':
        if value == './save':
            return 'data:/save'
    return value

def get_wave_sound(converter, data):
    import hashlib
    name = '%s.wav' % hashlib.md5(data).hexdigest()
    path = os.path.join(os.path.dirname(__file__), 'nl2', name)
    with open(path, 'rb') as fp:
        data = fp.read()
    return data

def use_blitter_callback(converter, obj):
    return True

def use_safe_create(converter):
    return True

LAYERS = {
    'Top': 0.0,
    'Middle': 0.5,
    'Bottom': 1.0
}

def get_depth(converter, layer):
    if converter.platform_name != '3ds':
        return None
    if not converter.current_frame.name.startswith('--'):
        return 0.0
    return LAYERS[layer.name]

has_func = set()

from chowdren.writers.events.system import get_loop_index_name

def prepare_loop_body(converter, loop_name, writer, groups):
    if loop_name != "box_touch_y1":
        return None
    obj = groups[0].conditions[1].get_object()
    obj_name = 'setup_loop_instance_%s_%s' % obj
    name = 'setup_loop_%s_%s' % obj
    if obj not in has_func:
        loop_index = get_loop_index_name(loop_name)
        writer.add_member('FrameObject * %s' % obj_name)
        writer.putmeth('void %s' % name)
        with converter.iterate_object(obj, writer, copy=False):
            obj_ref = converter.get_object(obj)
            writer.putlnc('if (%s->alterables->values.get(25) == %s) {',
                          obj_ref, loop_index)
            writer.indent()
            writer.putlnc('%s = %s;', obj_name, obj_ref)
            writer.putlnc('goto setup_end;')
            writer.end_brace()
        writer.put_label('setup_end')
        writer.end_brace()
        has_func.add(obj)
    klass = converter.get_object_class(obj[1])
    converter.set_object(obj, '((%s)%s)' % (klass, obj_name))
    return [name]
