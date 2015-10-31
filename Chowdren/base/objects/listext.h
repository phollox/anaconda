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

#ifndef CHOWDREN_LISTEXT_H
#define CHOWDREN_LISTEXT_H

#include "frameobject.h"
#include <string>
#include "datastream.h"
#include "types.h"

typedef vector<std::string> StringList;

class ListObject : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(ListObject)

    enum ListFlags
    {
        SORT_LIST = 1 << 0
    };

    StringList lines;
    int list_flags;
    int current_line;
    int index_offset;

    ListObject(int x, int y, int type_id);
    void load_file(const std::string & name);
    void add_line(const std::string & value);
    void set_line(int index, const std::string & value);
    void delete_line(int line);
    void clear();
    const std::string & get_line(int i);
    const std::string & get_current_line();
    int get_count();
    bool get_focus();
    void disable_focus();
    int find_string(const std::string & text, int flag);
    int find_string_exact(const std::string & text, int flag);
    void sort();
    void set_current_line(int index);
};

#endif // CHOWDREN_LISTEXT_H
