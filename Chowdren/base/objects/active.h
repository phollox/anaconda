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

#ifndef CHOWDREN_ACTIVE_H
#define CHOWDREN_ACTIVE_H

#include "frameobject.h"
#include "collision.h"
#include <string>
#include "color.h"

struct Direction
{
    signed char index;
    short min_speed, max_speed, back_to;
    short loop_count;
    short frame_count;
    Image ** frames;
};

struct Animation
{
    Direction * dirs[32];
};

struct Animations
{
    signed char count;
    Animation ** items;
};

class Active : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(Active)

#ifdef CHOWDREN_DEFER_COLLISIONS
    int old_aabb[4];
#endif

    enum ActiveFlags
    {
        ANIMATION_STOPPED = 1 << 0,
        AUTO_ROTATE = 1 << 1,
        TRANSPARENT = 1 << 2,
        DOOR_FADEOUT = 1 << 3
    };

    Animations * animations;

    signed char animation, forced_animation, current_animation,
                animation_finished;
    short animation_frame, forced_frame;
    signed char animation_direction, forced_direction;
    short forced_speed;
    short loop_count;
    unsigned int counter;
    float angle;
    float x_scale, y_scale;
    unsigned char active_flags;
    signed char last_dir;
    int action_x, action_y;
    SpriteCollision sprite_col;
    Direction * direction_data;
    Image * image;
    float flash_time, flash_interval;
    float fade_time, fade_duration;

#ifdef CHOWDREN_ACTIVE_REPLACE_COLOR
    ReplacedImages replacer;
#endif

    Active(int x, int y, int type_id);
    void initialize_active(bool collision_box);
    ~Active();
    void force_animation(int value);
    void force_frame(int value);
    void force_speed(int value);
    void force_direction(int value);
    void restore_direction();
    void restore_animation();
    void restore_frame();
    void restore_speed();
    void update_frame();
    void update_direction(Direction * dir = NULL);
    void update_action_point();
    void update();
    void draw();
    void draw_door_fadeout();
    int get_action_x();
    int get_action_y();
    void set_angle(float angle, int quality = 0);
    int get_frame();
    Direction * get_direction_data();
    int get_animation(int anim);
    void set_animation(int value);
    void set_direction(int value, bool set_movement = true);
    int get_animation_direction();
    void set_scale(float scale);
    void set_x_scale(float value);
    void set_y_scale(float value);
    void paste(int collision_type);
    bool test_animation(int value);
    void stop_animation();
    void start_animation();
    void flash(float value);
    bool is_animation_finished(int anim);
    void destroy();
    bool has_animation(int anim);
    void load(const std::string & filename, int anim, int dir, int frame,
              int hot_x, int hot_y, int action_x, int action_y,
              TransparentColor transparent_color);
    void replace_color(const Color & from, const Color & to);

    float get_angle()
    {
        return angle;
    }
};

extern FrameObject * default_active_instance;

#endif // CHOWDREN_ACTIVE_H
