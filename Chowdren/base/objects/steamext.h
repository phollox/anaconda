// Copyright (c) Mathias Kaerlev 2012-2015.
//
// This file is part of Anaconda.
//
// Anaconda is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Anaconda is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Anaconda.  If not, see <http://www.gnu.org/licenses/>.

#ifndef CHOWDREN_STEAMEXT_H
#define CHOWDREN_STEAMEXT_H

#include "chowstring.h"
#include "frameobject.h"
#include "types.h"

class SteamObject : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(SteamObject)

    SteamObject(int x, int y, int type_id);
    bool is_ready();
    void update();
    void unlock_achievement(const chowstring & name);
    void clear_achievement(const chowstring & name);
    void clear_achievements();
    bool is_achievement_unlocked(const chowstring & name);
    bool is_activated();

    void download(const chowstring & name);
    void download(const chowstring & path, int priority, int content_id,
                  Frame::EventFunction success, Frame::EventFunction fail);
    void upload(const chowstring & name);

    void request_user_data();
    void store_user_data();
    bool has_app(int id);

    struct SubResult
    {
        int index;
        chowstring title;
        chowstring cloud_path;
        int publish_id;
    };

    static SubResult sub_result;

    bool get_content(Frame::EventFunction loop, Frame::EventFunction finish);
    void set_search(bool subs);

    void set_int(const chowstring & name, int value);
    int get_int(const chowstring & name);

    int get_unlocked(const chowstring & name);

    const chowstring & get_user_name();
    int get_user_id();

    bool is_active(const chowstring & session_id);
    bool is_connected();

    void reset_uncommited_changes();
    void reset_file_changes();
    void reset_files();
    void reset_changes();
    void set_preview_latest(const chowstring & local_path,
                            const chowstring & cloud_path,
                            bool overwrite);
    void upload_changes(Frame::EventFunction done, Frame::EventFunction fail);
    void set_tags(const chowstring & tags);
    void set_description(const chowstring & value);
    void set_file(const chowstring & local_path,
                  const chowstring & cloud_path,
                  bool overwrite);
    void set_content_title(const chowstring & title);
    void set_content_appid(unsigned int id);
    void set_content_visibility(int value);
    void start_content_change(unsigned int content_id,
                              const chowstring & session_id);
    void start_publish(const chowstring & session_id);
    const chowstring & get_error();

    bool is_enabled();

#ifdef CHOWDREN_IS_FP
    void find_board(int char_id, int stage_id);
    void upload_crystal(int value);
    void upload_time(int value);
#endif
};

#endif // CHOWDREN_STEAMEXT_H
