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

#ifndef CHOWDREN_MANAGER_H
#define CHOWDREN_MANAGER_H

#include "events.h"
#include "globals.h"
#include "color.h"
#include "fpslimit.h"
#include "include_gl.h"
#include "input.h"
#include "pool.h"
#include "chowconfig.h"
#include "transition.h"

class Frame;

class GameManager
{
public:
    Frame * frame;
#ifdef CHOWDREN_SUBAPP_FRAMES
    Frame * main_frame;
#endif
    GlobalValues * values;
    GlobalStrings * strings;
    FPSLimiter fps_limit;
    bool window_created;
    bool app_fullscreen, fullscreen;
    int off_x, off_y, x_size, y_size;
    int mouse_x, mouse_y;

    // transition
    Color fade_color;
    float fade_dir;
    float fade_value;
    Transition::Type fade_type;

    int score;
    int lives;
    bool player_died;
    float dt;
    float timer_mul;
#if CHOWDREN_IS_DEMO
    bool idle_timer_started;
    double idle_timer;
    double global_time;
    double show_build_timer;
    double reset_timer;
    double manual_reset_timer;
#endif
    InputList keyboard;
    InputList mouse;

#if defined(CHOWDREN_USE_EDITOBJ) || defined(CHOWDREN_USE_GWEN)
    std::string input;
#endif

    // player controls
    int up, down, left, right, button1, button2, button3, button4;

    int player_flags, player_press_flags;
    int control_type;
    bool ignore_controls;

    // joystick controls
    int joystick_flags, joystick_press_flags, joystick_release_flags;

    GameManager();
    void init();
    void on_key(int key, bool state);
    void on_mouse(int key, bool state);
    bool update();
    int update_frame();
    void draw();
    void set_frame(int index);
    void set_framerate(int framerate);
    void set_window(bool fullscreen);
    void set_window_scale(int scale);
    void set_scale_type(int type);
    void set_fullscreen_type(int type);
    bool is_fullscreen();
    void run();
    void reset_globals();
    void set_fade(Transition::Type type, const Color & color, float dir);
    void draw_fade();

#ifdef CHOWDREN_USE_JOYTOKEY
    int key_mappings[CHOWDREN_BUTTON_MAX-1];
    float deadzone;
    int axis_pos_mappings[CHOWDREN_AXIS_MAX-1];
    int axis_neg_mappings[CHOWDREN_AXIS_MAX-1];
    int axis_values[CHOWDREN_AXIS_MAX-1];
    int last_axis;
    bool axis_moved;
    bool pad_selected;
    bool pad_disconnected;

    int simulate_count;
    struct SimulateKey
    {
        int key;
        bool down;

        SimulateKey()
        : key(-1), down(false)
        {
        }
    };

    SimulateKey simulate_keys[InputList::STATE_COUNT];

    void set_deadzone(float deadzone);
    void simulate_key(const std::string & key);
    void simulate_key(int key);
    void map_button(int button, const std::string & key);
    void map_axis(int axis, const std::string & neg, const std::string & pos);
    void reset_map();
#endif
};

inline FrameObject * get_instance(ObjectList & list)
{
    return list.back_obj;
}

inline FrameObject * get_instance(ObjectList & list, int index)
{
    if (list.empty())
        return NULL;
    int size = list.size();
    index = (size - 1) - (index % size);
    return list[index];
}

inline FrameObject * get_instance(ObjectList & list, FrameObject * def)
{
    if (list.back_obj == NULL)
        return def;
    return list.back_obj;
}

inline FrameObject * get_instance(ObjectList & list, int index,
                                  FrameObject * def)
{
    if (list.empty())
        return def;
    index = (list.size() - 1) - (index % list.size());
    return list[index];
}

inline FrameObject * get_qualifier(QualifierList & list)
{
    return list.back();
}

inline FrameObject * get_qualifier(QualifierList & list, int index)
{
    if (list.empty())
        return NULL;
    int size = list.size();
    index = (size - 1) - (index % size);
    return list[index];
}

inline FrameObject * get_qualifier(QualifierList & list, FrameObject * def)
{
    FrameObject * ret = list.back();
    if (ret == NULL)
        return def;
    return ret;
}

inline FrameObject * get_qualifier(QualifierList & list, int index,
                                   FrameObject * def)
{
    int size = list.size();
    if (size == 0)
        return def;
    index = (size - 1) - (index % size);
    return list[index];
}

extern GameManager manager;

#endif // CHOWDREN_MANAGER_H
