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

#ifndef CHOWDREN_BINARYEXT_H
#define CHOWDREN_BINARYEXT_H

#include "frameobject.h"
#include <string>

class BinaryObject : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(BinaryObject)

    char * data;
    size_t size;

    BinaryObject(int x, int y, int type_id);
    ~BinaryObject();
    void load_file(const std::string & filename);
    void save_file(const std::string & filename);
    void set_byte(unsigned char value, size_t addr);
    void resize(size_t size);
    int get_byte(size_t addr);
    int get_short(size_t addr);
};

#endif // CHOWDREN_BINARYEXT_H
