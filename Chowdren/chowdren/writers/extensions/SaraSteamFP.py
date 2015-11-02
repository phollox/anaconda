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

from chowdren.writers.events import (ConditionMethodWriter,
    ActionMethodWriter, ExpressionMethodWriter, make_table,
    TrueCondition, FalseCondition, EmptyAction)

class SteamObject(ObjectWriter):
    class_name = 'SteamObject'
    update = False
    filename = 'steamext'

    def write_init(self, writer):
        pass

actions = make_table(ActionMethodWriter, {
    0 : 'update', # general_frame_update_0
    1 : 'find_board', # leaderboards_uploading_find_boards_1
    2 : 'upload_crystal', # leaderboards_uploading_upload_crystals_2
    3 : 'upload_time', # leaderboards_uploading_upload_time_3
    6 : 'unlock_achievement', # achievements_simple_unlock_6
    7 : 'clear_achievement', # achievements_simple_lock_7
    11 : 'upload("./records.dat")', # cloud_simple_upload_records_dat_11
    13 : 'upload("./control_gamepad.cfg")', # cloud_simple_upload_gamepad_cfg_13
    14 : 'upload("./control_keyboard.cfg")', # cloud_simple_upload_keyboard_cfg_14
    15 : 'download("./records.dat")', # cloud_simple_download_records_dat_15
    17 : 'download("./control_gamepad.cfg")', # cloud_simple_download_gamepad_cfg_17
    18 : 'download("./control_keyboard.cfg")', # cloud_simple_download_keyboard_cfg_18
    27 : 'upload("./file" + number_to_string(%s) + ".sav")', # cloud_simple_upload_adventure_27
    29 : 'download("./file" + number_to_string(%s) + ".sav")' # cloud_simple_download_adventure_29
})

conditions = make_table(ConditionMethodWriter, {
})

expressions = make_table(ExpressionMethodWriter, {
})

def get_object():
    return SteamObject