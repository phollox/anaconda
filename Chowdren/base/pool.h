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

#ifndef CHOWDREN_POOL_H
#define CHOWDREN_POOL_H

#include "types.h"
#include <stdlib.h>

/*
NOTE: This intentionally leaks the blocks used, as we intend to use them for
the duration of the application.
*/

template <class T>
class ObjectPool
{
public:
    void ** free_items;
    long available;
    long total;
    long buffer_size;

    ~ObjectPool()
    {
        delete[] free_items;
    }

    void * create()
    {
        if (available > 0)
            return free_items[--available];

        if (buffer_size == 0)
            buffer_size = 32;

        delete[] free_items;
        unsigned char * block = new unsigned char[sizeof(T)*buffer_size];
        total += buffer_size;
        free_items = new void*[total];
        for (int i = 0; i < buffer_size; i++) {
            free_items[available++] = block;
            block += sizeof(T);
        }
        buffer_size *= 2;
        return free_items[--available];
    }

    void destroy(void * ptr)
    {
        if (ptr == NULL)
            return;
        free_items[available++] = ptr;
    }
};

#endif // CHOWDREN_POOL_H
