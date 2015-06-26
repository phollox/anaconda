#ifndef CHOWDREN_MOVEIT_H
#define CHOWDREN_MOVEIT_H

#include "frameobject.h"

class MoveIt
{
public:
    static FlatObjectList queue;

    static void add_queue(QualifierList & objs);
    static void move(int x, int y, int speed);
    static void clear_queue();
    static void stop(QualifierList & objs);
};

#endif // CHOWDREN_MOVEIT_H
