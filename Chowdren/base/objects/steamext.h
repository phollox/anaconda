#ifndef CHOWDREN_STEAMEXT_H
#define CHOWDREN_STEAMEXT_H

#include <string>
#include "frameobject.h"

class SteamObject : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(SteamObject)

    SteamObject(int x, int y, int type_id);
    bool is_ready();
    void update();
    void unlock_achievement(const std::string & name);
    void clear_achievement(const std::string & name);
    bool is_achievement_unlocked(const std::string & name);

    void download(const std::string & name);
    void upload(const std::string & name);

#ifdef CHOWDREN_IS_FP
    void find_board(int char_id, int stage_id);
    void upload_crystal(int value);
    void upload_time(int value);
#endif
};

#endif // CHOWDREN_STEAMEXT_H
