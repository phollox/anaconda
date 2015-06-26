#include "objects/pathplanner.h"
#include "collision.h"
#include "objects/pathfinding/jps.h"

PathPlanner::PathPlanner(int x, int y, int type_id)
: FrameObject(x, y, type_id)
{
}

PathPlanner::~PathPlanner()
{
    free(map.data);
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
        if (node.x != agent.x || node.y != agent.y)
            continue;
        agent.nodes.pop_back();
    }
}

void PathPlanner::add_agent(FrameObject * obj)
{
    if (obj->agent == NULL)
        obj->agent = new PathAgent();
    obj->agent->planner = this;
    obj->agent->obj = obj;
    obj->agent->x = to_grid(obj->x);
    obj->agent->y = to_grid(obj->y);
    agents.push_back(obj);
}

void PathPlanner::add_obstacle(FrameObject * obj)
{
    InstanceCollision * c = obj->collision;
    if (c == NULL)
        return;

    int aabb[4];
    for (int i = 0; i < 4; ++i)
        aabb[i] = to_grid(c->aabb[i]);

    aabb[0] = std::max(0, to_grid(c->aabb[0]));
    aabb[1] = std::max(0, to_grid(c->aabb[1]));
    aabb[2] = std::min(map_width, to_grid(c->aabb[2]));
    aabb[3] = std::min(map_height, to_grid(c->aabb[3]));

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
    int dest_x = planner->to_pixels(node.x) + planner->tile_size / 2;
    int dest_y = planner->to_pixels(node.y) + planner->tile_size / 2;
    int dir = get_direction_int(obj->get_x(), obj->get_y(), dest_x, dest_y);
    obj->set_direction(dir);
}

struct Grid
{
    PathPlanner & planner;

    Grid(PathPlanner & planner)
    : planner(planner)
    {
    }

    inline bool operator()(unsigned x, unsigned y) const
    {
        if (x >= (unsigned)planner.map_width ||
            y >= (unsigned)planner.map_height)
            return false;
        int i = planner.to_index((int)x, (int)y);
        return planner.map.get(i) == 0;
    }
};

void PathPlanner::plan_path(FrameObject * obj)
{
    // XXX this is stupid and slow, but should work well enough
    PathAgent & agent = *obj->agent;
    PathPlanner * planner = (PathPlanner*)agent.planner;
    agent.nodes.clear();
    Grid grid(*planner);
    unsigned step = 0;
    JPS::PathVector path;
    bool found = JPS::findPath(path, grid, agent.x, agent.y,
                               agent.dest_x, agent.dest_y, step);
    if (!found)
        return;
    JPS::PathVector::iterator it;
    for (it = path.begin(); it != path.end(); ++it) {
        PathNode n = {it->x, it->y};
        agent.nodes.push_back(n);
    }
}

FrameObject::PathAgent::PathAgent()
: dest_x(0), dest_y(0)
{
}

FrameObject::PathAgent::~PathAgent()
{
    PathPlanner * p = (PathPlanner*)planner;
    p->agents.erase(std::remove(p->agents.begin(), p->agents.end(), obj),
                    p->agents.end());
}

bool FrameObject::PathAgent::at_destination()
{
    return nodes.empty();
}

bool FrameObject::PathAgent::not_at_destination()
{
    return !nodes.empty();
}

bool FrameObject::PathAgent::is_stopping()
{
    return nodes.empty();
}

