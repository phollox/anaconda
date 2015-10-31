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

#include "objects/pathplanner.h"
#include "collision.h"
#include "objects/pathfinding/jps.h"
#include "movement.h"

PathPlanner::PathPlanner(int x, int y, int type_id)
: FrameObject(x, y, type_id)
{
}

PathPlanner::~PathPlanner()
{
    free(map.data);
    FlatObjectList::iterator it;
    for (it = agents.begin(); it != agents.end(); ++it) {
        FrameObject * obj = *it;
        obj->agent->planner = NULL;
        delete obj->agent;
        obj->agent = NULL;
    }
    agents.clear();
}

void PathPlanner::create_map()
{
    int size = GET_BITARRAY_SIZE(map_width * map_height);
    map.data = (BaseBitArray::word_t*)malloc(size);
    memset(map.data, 0, size);
}

void PathPlanner::update()
{
    FlatObjectList::iterator it;
    for (it = agents.begin(); it != agents.end(); ++it) {
        FrameObject * obj = *it;
        PathAgent & agent = *obj->agent;
        agent.x = to_grid(obj->x);
        agent.y = to_grid(obj->y);
        if (agent.nodes.empty())
            continue;
        PathNode & node = agent.nodes.back();
        if (agent.x == node.x && agent.y == node.y) {
            agent.nodes.pop_back();
        }
    }
}

void PathPlanner::add_agent(FrameObject * obj)
{
    if (obj->agent == NULL)
        obj->agent = new PathAgent();
    obj->agent->planner = this;
    obj->agent->obj = obj;
    obj->agent->x = obj->agent->dest_x = to_grid(obj->x);
    obj->agent->y = obj->agent->dest_y = to_grid(obj->y);
    agents.push_back(obj);
}

void PathPlanner::add_obstacle(FrameObject * obj)
{
    InstanceCollision * c = obj->collision;
    if (c == NULL)
        return;
    int aabb[4];
    aabb[0] = std::max(0, c->aabb[0] / tile_size);
    aabb[1] = std::max(0, c->aabb[1] / tile_size);
    aabb[2] = std::min(map_width, (c->aabb[2]-1) / tile_size);
    aabb[3] = std::min(map_height, (c->aabb[3]-1) / tile_size);

    for (int y = aabb[1]; y <= aabb[3]; ++y)
    for (int x = aabb[0]; x <= aabb[2]; ++x) {
        map.set(to_index(x, y));
    }
}

void PathPlanner::set_destination(FrameObject * obj, int x, int y)
{
    PathAgent & agent = *obj->agent;
    PathPlanner * planner = (PathPlanner*)agent.planner;
    obj->agent->dest_x = planner->to_grid(x);
    obj->agent->dest_y = planner->to_grid(y);
}

void PathPlanner::orient(FrameObject * obj)
{
    PathAgent & agent = *obj->agent;
    if (agent.nodes.empty())
		return;
	PathPlanner * planner = (PathPlanner*)agent.planner;
    PathNode & node = agent.nodes.back();
    int node_x = node.x * planner->tile_size
		         + planner->tile_size / 2;
    int node_y = node.y * planner->tile_size
		         + planner->tile_size / 2;
    if (get_distance(obj->x, obj->y, node_x, node_y) < 1.0f)
        return;
    int dir = get_direction_int(obj->get_x(), obj->get_y(),
                                node_x, node_y);
    obj->set_direction(dir);
}

struct Grid
{
    PathPlanner & planner;
    FrameObject::PathAgent & agent;
    unsigned dest_x, dest_y;
    unsigned src_x, src_y;

    Grid(PathPlanner & planner, FrameObject::PathAgent & agent)
    : planner(planner), agent(agent),
      dest_x(agent.dest_x),
      dest_y(agent.dest_y),
      src_x(agent.x),
      src_y(agent.y)
    {
    }

    inline bool operator()(unsigned x, unsigned y) const
    {
        if (x >= (unsigned)planner.map_width ||
            y >= (unsigned)planner.map_height)
            return false;
        if (x == dest_x && y == dest_y)
            return true;
        if (x == src_x && y == src_y)
            return true;
        int i = planner.to_index((int)x, (int)y);
        return planner.map.get(i) == 0;
    }
};

#include "collision.h"

static void dump_map(int dest_x, int dest_y, PathPlanner * planner)
{
    // std::string v = number_to_string(dest_x) + "_" + number_to_string(dest_y)
    //                 + ".dat";
    save_bitarray("dump.dat", planner->map,
                  planner->map_width, planner->map_height);
}

void PathPlanner::plan_path(FrameObject * obj)
{
    // XXX this is stupid and slow, but should work well enough
    FrameObject::PathAgent & agent = *obj->agent;
    agent.nodes.clear();
    PathPlanner * planner = (PathPlanner*)agent.planner;
    if (agent.x == agent.dest_x && agent.y == agent.dest_y)
        return;
    int grid_x = clamp(agent.dest_x,
                       0, planner->map_width - 1);
    int grid_y = clamp(agent.dest_y,
                       0, planner->map_height - 1);
    int test = planner->to_index(grid_x, grid_y);
    if (planner->map.get(test)) {
#ifndef NDEBUG
        dump_map(grid_x, grid_y, planner);
#endif
        std::cout << "Destination not possible" << std::endl;
        return;
    }
    Grid grid(*planner, agent);
    unsigned step = 1;
    JPS::PathVector path;
    bool found = JPS::findPath(path, grid, agent.x, agent.y,
                               agent.dest_x, agent.dest_y, step);
    if (!found) {
        std::cout << "No path found" << std::endl;
        return;
    }
    JPS::PathVector::iterator it;
    for (int i = 0; i < int(path.size()); ++i) {
        JPS::Position & node = path[i];
        PathNode n = {node.x, node.y};
        agent.nodes.push_back(n);
    }
}

FrameObject::PathAgent::PathAgent()
: dest_x(0), dest_y(0), flags(0), node_reached(0)
{
}

FrameObject::PathAgent::~PathAgent()
{
    PathPlanner * p = (PathPlanner*)planner;
    if (p == NULL)
        return;
    p->agents.erase(std::remove(p->agents.begin(), p->agents.end(), obj),
                    p->agents.end());
}

bool FrameObject::PathAgent::at_destination()
{
    return x == dest_x && y == dest_y;
}

bool FrameObject::PathAgent::not_at_destination()
{
    return !at_destination();
}

bool FrameObject::PathAgent::is_stopping()
{
    return nodes.empty();
}

