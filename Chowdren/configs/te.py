from chowdren.writers.events.system import get_loop_index_name
from chowdren.common import is_qualifier

def use_deferred_collisions(converter):
    return False

def init(converter):
    converter.add_define('CHOWDREN_IS_TE')
    converter.add_define('CHOWDREN_QUICK_SCALE')
    converter.add_define('CHOWDREN_POINT_FILTER')
    converter.add_define('CHOWDREN_OBSTACLE_IMAGE')
    converter.add_define('CHOWDREN_TEXTURE_GC')
    converter.add_define('CHOWDREN_SPECIAL_POINT_FILTER')
    converter.add_define('CHOWDREN_JOYSTICK2_CONTROLLER')
    converter.add_define('CHOWDREN_FORCE_X360')
    converter.add_define('CHOWDREN_FORCE_TRANSPARENT')
    converter.add_define('CHOWDREN_FORCE_TEXT_LAYOUT')
    converter.add_define('CHOWDREN_PASTE_PRECEDENCE')
    converter.add_define('CHOWDREN_STEAM_APPID', 298630)
    converter.add_define('CHOWDREN_TEXT_USE_UTF8')
    converter.add_define('CHOWDREN_INI_USE_UTF8')
    converter.add_define('CHOWDREN_FORCE_STEAM_OPEN')
    converter.add_define('CHOWDREN_USE_STEAM_LANGUAGE')
    converter.add_define('CHOWDREN_FORCE_FILL')

def write_frame_post(converter, writer):
    if converter.current_frame_index != 0:
        return
    writer.putlnc('std::string lang = platform_get_language();')
    langs = {
        'English': 'eng',
        'Russian': 'rus',
        'Spanish': 'spa',
        'German': 'ger',
        'French': 'fre',
        'Polish': 'pol'
    }
    for k, v in langs.iteritems():
        writer.putlnc('if (lang == %r) lang = %r;', k, v, cpp=False)
    writer.putlnc('global_strings->set(6, lang);')

def init_obj(converter, obj):
    if obj.data.name == 'Dialogue 2':
        # we could use use_iteration_index, but let's use a simple fix
        obj.common.text.items[0].value = ''
    elif obj.data.name == 'String 3':
        item = obj.common.text.items[0]
        if item.value == 'DEBUG MODE ENABLED':
            item.value = ''
    elif obj.data.name == 'LoS':
        # stupid F2.5 bug
        obj.common.newFlags['CollisionBox'] = False
    elif obj.data.name == 'soil background':
        # XXX nasty hack, but probably better this way
        obj.common.newFlags['ObstacleSolid'] = False

def use_image_preload(converter):
    return True

def use_image_flush(converter, frame):
    return False

def use_edit_obj(converter):
    return True

alterable_int_objects = [
    ('BASEYou_', [3, 9]),
    ('BASEInmate_', [3, 9]),
    ('BASEGuard_', [3, 9])
]

alterable_int_quals = [
    ((32815, 2), [3, 9])
]

alt_qual_names = ("BASE - Inmate", "BASE - Guard")

def get_fonts(converter):
    return ('Escapists',)

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

    for (check_name, alts) in alterable_int_quals:
        if not obj == check_name:
            continue
        if alts is None:
            return True
        index = expression.data.loader.value
        return index in alts

    return False