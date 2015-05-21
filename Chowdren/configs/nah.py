from chowdren.writers.events.system import get_loop_index_name

def use_deferred_collisions(converter):
    return False

def init(converter):
    converter.add_define('CHOWDREN_IS_NAH')
    converter.add_define('CHOWDREN_QUICK_SCALE')
    converter.add_define('CHOWDREN_POINT_FILTER')
    converter.add_define('CHOWDREN_OBSTACLE_IMAGE')
    converter.add_define('CHOWDREN_TEXTURE_GC')
    converter.add_define('CHOWDREN_CACHE_INI')
    converter.add_define('CHOWDREN_PASTE_REMOVE')
    converter.add_define('CHOWDREN_ACTIVE_REPLACE_COLOR')
    converter.add_define('CHOWDREN_ACTIVE_LOAD_SINGLE')
    converter.add_define('CHOWDREN_ACTIVE_LOOPING_APPEARING')
    converter.add_define('CHOWDREN_SPECIAL_POINT_FILTER')
    converter.add_define('CHOWDREN_JOYSTICK2_CONTROLLER')
    converter.add_define('CHOWDREN_FORCE_X360')
    converter.add_define('CHOWDREN_PASTE_CACHE')
    converter.add_define('CHOWDREN_DEFAULT_SCALE', 2)
    # converter.add_define('CHOWDREN_PASTE_BROADPHASE')

def get_loop_name(converter, parameter):
    name = converter.convert_parameter(parameter)
    if name == 'global_strings->get(0)':
        return ('generic_ai_update', 'SWAT', 'NINJA')
    return None

def get_dynamic_loop_index(converter, exp):
    if exp.getName() != 'GlobalString':
        return None
    return 'generic_ai_update'

def get_dynamic_loop_call_name(converter, parameter):
    name = converter.convert_parameter(parameter)
    if name == 'global_strings->get(0)':
        items = parameter.parent.parent.actions[0].items[1].loader.items
        name = converter.convert_static_expression(items)
        if name in converter.system_object.loops:
            return name
        return 'generic_ai_update'


def write_loop(converter, loop_name, event_writer, writer):
    if loop_name not in ('SWAT', 'NINJA'):
        return
    ai_name = get_loop_index_name('generic_ai_update')
    index_name = get_loop_index_name(loop_name)
    event_writer.group.add_member('int %s' % ai_name, 0)
    writer.putlnc('%s = %s;', ai_name, index_name)

def use_image_preload(converter):
    return True

def use_image_flush(converter, frame):
    return False

alterable_int_objects = [
    ('SCOREVARIABLES_', [9, 10]),
    ('GOALOBJECT_', [9]),
    ('NEWSCREW_', [1]),
    ('PAYPHONE_', [1])
]

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

def is_controller(value):
    if not value.startswith('((Active*)get_instance(controller_'):
        return False
    if not value.endswith('_instances))->alterables->values.get(12)'):
        return False
    return True

def is_global_fixed(value):
    return value.startswith('global_values->get(15)')

def write_pre(converter, writer, group):
    for condition in group.conditions:
        if condition.data.getName() != 'CompareFixedValue':
            continue
        value = condition.convert_index(0)
        if not is_controller(value) and not is_global_fixed(value):
            continue
        break
    else:
        return

    condition.force_single = True
    group.conditions.remove(condition)
    if converter.is_triggered:
        group.conditions.insert(1, condition)
    else:
        group.conditions.insert(0, condition)

def get_string(converter, value):
    if value == 'XBOX':
        return 'X360'
    return value
