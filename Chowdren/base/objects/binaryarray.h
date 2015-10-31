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

#ifndef CHOWDREN_BINARYARRAY_H
#define CHOWDREN_BINARYARRAY_H

#include "frameobject.h"
#include <string>
#include "datastream.h"
#include "types.h"

class Workspace
{
public:
    std::string name;
    std::stringstream data;

    Workspace(FileStream & stream);
    Workspace(const std::string & name);
};

typedef hash_map<std::string, Workspace*> WorkspaceMap;

class BinaryArray : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(BinaryArray)

    WorkspaceMap workspaces;
    Workspace * current;

    BinaryArray(int x, int y, int type_id);
    ~BinaryArray();
    void load_workspaces(const std::string & filename);
    void create_workspace(const std::string & name);
    void switch_workspace(const std::string & name);
    void switch_workspace(Workspace * workspace);
    bool has_workspace(const std::string & name);
    void load_file(const std::string & filename);
    std::string read_string(int pos, size_t size);
    size_t get_size();
};

#endif // CHOWDREN_BINARYARRAY_H
