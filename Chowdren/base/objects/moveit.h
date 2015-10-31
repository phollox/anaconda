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

#ifndef CHOWDREN_MOVEIT_H
#define CHOWDREN_MOVEIT_H

#include "frameobject.h"
#include "types.h"

class MoveIt : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(MoveIt)

    static FlatObjectList queue;
    static FlatObjectList instances;

    MoveIt(int x, int y, int type_id);

    static void add_queue(QualifierList & objs);
    static void move(int x, int y, int speed);
    static void clear_queue();
    static void stop(QualifierList & objs);
    void update();
};

#endif // CHOWDREN_MOVEIT_H
