from chowdren.writers.events.system import get_loop_index_name

def use_deferred_collisions(converter):
    return False

def init(converter):
    converter.game.header.windowWidth = 1280
    converter.game.header.windowHeight = 800
    converter.add_define('CHOWDREN_IS_TE')
    converter.add_define('CHOWDREN_IS_TEED')
    converter.add_define('CHOWDREN_QUICK_SCALE')
    converter.add_define('CHOWDREN_OBSTACLE_IMAGE')
    converter.add_define('CHOWDREN_TEXTURE_GC')
    converter.add_define('CHOWDREN_JOYSTICK2_CONTROLLER')
    converter.add_define('CHOWDREN_FORCE_X360')
    converter.add_define('CHOWDREN_FORCE_TRANSPARENT')
    converter.add_define('CHOWDREN_FORCE_TEXT_LAYOUT')
    converter.add_define('CHOWDREN_SUBAPP_FRAMES')
    converter.add_define('CHOWDREN_EMULATE_MENU')
    converter.add_define('CHOWDREN_PICTURE_OFFSET')
    converter.add_define('CHOWDREN_AUTOSAVE_ON_CHANGE')
    converter.add_define('CHOWDREN_NO_FULLSCREEN')
    converter.add_define('CHOWDREN_STEAM_APPID', 298630)
    converter.add_define('CHOWDREN_TEXT_USE_UTF8')
    converter.add_define('CHOWDREN_INI_USE_UTF8')
    converter.add_define('CHOWDREN_USE_STEAM_LANGUAGE')
    # converter.add_define('CHOWDREN_FORCE_STEAM_OPEN')

    values = converter.game.globalValues.items
    values[7] = 0 # turn off dev mode


def use_subapp_frames(converter):
    return True

def use_image_preload(converter):
    return True

def use_image_flush(converter, frame):
    return False

def use_edit_obj(converter):
    return True

def use_gwen(converter):
    return True

def get_fonts(converter):
    return ('SegoeUI',)
