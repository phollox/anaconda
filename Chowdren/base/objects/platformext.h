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

#ifndef CHOWDREN_PLATFORMEXT_H
#define CHOWDREN_PLATFORMEXT_H

#include "frameobject.h"

typedef void (*ObstacleOverlapCallback)();
typedef void (*PlatformOverlapCallback)();

class PlatformObject : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(PlatformObject)

    FrameObject * instance;
    bool left, right;
    bool paused;

    int x_vel, y_vel;
    int max_x_vel, max_y_vel;
    int add_x_vel, add_y_vel;
    int x_move_count, y_move_count;
    int x_accel, x_decel;
    int gravity;
    int jump_strength;
    int jump_hold_height;
    int step_up;
    int slope_correction;
    bool on_ground;
    bool jump_through, through_collision_top;

    bool obstacle_collision;
    bool platform_collision;

    ObstacleOverlapCallback obstacle_callback;
    PlatformOverlapCallback platform_callback;

    PlatformObject(int x, int y, int type_id);
    void set_object(FrameObject * instance);
    virtual void call_overlaps_obstacle();
    virtual void call_overlaps_platform();
    bool overlaps_obstacle();
    bool overlaps_platform();
    bool is_falling();
    bool is_jumping();
    bool is_moving();
    void jump();
    void jump_in_air();
    void update();
    void set_y_vel(int value);
    void set_add_y_vel(int value);
    // void set_x_vel(int value);
    // void set_max_x_vel(int value);
    // void set_x_accel(int value);
};

extern FrameObject * default_platform_instance;

#endif // CHOWDREN_PLATFORMEXT_H
