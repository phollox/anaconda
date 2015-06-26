#include "objects/moveit.h"
#include "mathcommon.h"
#include "movement.h"

void MoveIt::add_queue(QualifierList & objs)
{
    for (QualifierIterator it(objs); !it.end(); ++it) {
        queue.push_back(*it);
    }
}

void MoveIt::move(int x, int y, int speed)
{
    FlatObjectList::iterator it;
    for (it = queue.begin(); it != queue.end(); ++it) {
        FrameObject * obj = *it;
        int dst_x = x - obj->layer->off_x;
        int dst_y = y - obj->layer->off_y;
        float distance = get_distance(obj->x, obj->y, dst_x, dst_y);
        int cycles = int(distance / speed);
        delete obj->movement;
        obj->movement = new MoveItMovement(obj, dst_x, dst_y, cycles);
        obj->movement->start();
    }
}

void MoveIt::clear_queue()
{
    queue.clear();
}

void MoveIt::stop(QualifierList & objs)
{
    for (QualifierIterator it(objs); !it.end(); ++it) {
        (*it)->clear_movements();
    }
}

FlatObjectList MoveIt::queue;
