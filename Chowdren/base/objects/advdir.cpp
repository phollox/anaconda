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

#include "objects/advdir.h"
#include "mathcommon.h"

// AdvancedDirection

AdvancedDirection::AdvancedDirection(int x, int y, int type_id)
: FrameObject(x, y, type_id)
{
}

void AdvancedDirection::find_closest(ObjectList & instances, int x, int y)
{
    float lowest_dist;
    closest = NULL;
    for (ObjectIterator it(instances); !it.end(); ++it) {
        FrameObject * instance = *it;
        float dist = get_distance(x, y, instance->x, instance->y);
        if (closest != NULL && dist > lowest_dist)
            continue;
        closest = instance;
        lowest_dist = dist;
    }
}

void AdvancedDirection::find_closest(QualifierList & instances, int x, int y)
{
    float lowest_dist;
    closest = NULL;
    for (QualifierIterator it(instances); !it.end(); ++it) {
        FrameObject * instance = *it;
        float dist = get_distance(x, y, instance->x, instance->y);
        if (closest != NULL && dist > lowest_dist)
            continue;
        closest = instance;
        lowest_dist = dist;
    }
}

FixedValue AdvancedDirection::get_closest(int n)
{
    return closest->get_fixed();
}

float AdvancedDirection::get_object_angle(FrameObject * a, FrameObject * b)
{
    return ::get_angle(a->x, a->y, b->x, b->y);
}
