from chowdren.writers.objects import ObjectWriter

from chowdren.common import get_animation_name, to_c, make_color

from chowdren.writers.events import (ConditionMethodWriter,
    ActionMethodWriter, ExpressionMethodWriter, make_table,
    TrueCondition, FalseCondition, EmptyAction)

class SteamObject(ObjectWriter):
    class_name = 'SteamObject'
    update = False
    filename = 'steamext'

    def write_init(self, writer):
        pass

    def has_sleep(self):
        return False

class SearchSubscribedItems(ActionMethodWriter):
    custom = True

    def write(self, writer):
        print 'SearchSubscribedItems not implemented yet'

actions = make_table(ActionMethodWriter, {
    15 : 'request_user_data',
    16 : 'store_user_data',
    17 : 'set_int',
    22 : 'unlock_achievement',
    26 : 'clear_achievements',
    158 : SearchSubscribedItems,
    162 : 'download', # download ugc
})

conditions = make_table(ConditionMethodWriter, {
    21 : 'is_achievement_unlocked',
    7 : 'has_app'
})

expressions = make_table(ExpressionMethodWriter, {
    2 : 'get_user_name()',
    1 : 'get_user_id()',
    7 : 'get_int',
    12 : 'get_unlocked'
})

def get_object():
    return SteamObject
