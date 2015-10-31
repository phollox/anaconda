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

#ifndef CHOWDREN_PATHPLANNER_H
#define CHOWDREN_PATHPLANNER_H

#include "frameobject.h"
#include "bitarray.h"

class PathPlanner : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(PathPlanner)

    int tile_size;
    int map_width, map_height;
    BitArray map;
    FlatObjectList agents;

    PathPlanner(int x, int y, int type_id);
    ~PathPlanner();
    void create_map();
    void update();
    void add_agent(FrameObject * obj);
    void add_obstacle(FrameObject * obj);

    inline int to_grid(int v)
    {
        return v / tile_size;
    }

    inline int to_pixels(int v)
    {
        return v * tile_size;
    }

    inline int to_index(int x, int y)
    {
        return x + y * map_width;
    }

    static void set_destination(FrameObject * obj, int x, int y);
    static void orient(FrameObject * obj);
    static void plan_path(FrameObject * obj);
};

#endif // CHOWDREN_PATHPLANNER_H
