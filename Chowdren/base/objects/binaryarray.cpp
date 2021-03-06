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

#include "objects/binaryarray.h"
#include "chowconfig.h"
#include "fileio.h"
#include "datastream.h"

// Workspace

Workspace::Workspace(FileStream & stream)
{
    stream.read_c_string(name);
    stream.read(data, stream.read_uint32());
}

Workspace::Workspace(const chowstring & name)
: name(name)
{
}

// BinaryArray

BinaryArray::BinaryArray(int x, int y, int type_id)
: FrameObject(x, y, type_id), current(NULL)
{

}

void BinaryArray::load_workspaces(const chowstring & filename)
{
    FSFile fp(convert_path(filename).c_str(), "r");
    FileStream stream(fp);
    Workspace * workspace;
    while (!stream.at_end()) {
        workspace = new Workspace(stream);
        workspaces[workspace->name] = workspace;
    }
    fp.close();
    switch_workspace(current);
}

BinaryArray::~BinaryArray()
{
    WorkspaceMap::const_iterator it;
    for (it = workspaces.begin(); it != workspaces.end(); ++it)
        delete it->second;
}

void BinaryArray::create_workspace(const chowstring & name)
{
    if (workspaces.find(name) != workspaces.end())
        return;
    Workspace * workspace = new Workspace(name);
    workspaces[name] = workspace;
}

void BinaryArray::switch_workspace(const chowstring & name)
{
    WorkspaceMap::const_iterator it = workspaces.find(name);
    if (it == workspaces.end())
        return;
    switch_workspace(it->second);
}

void BinaryArray::switch_workspace(Workspace * workspace)
{
    current = workspace;
}

bool BinaryArray::has_workspace(const chowstring & name)
{
    return workspaces.count(name) > 0;
}

void BinaryArray::load_file(const chowstring & filename)
{
    size_t size;
    char * data;
    read_file(convert_path(filename).c_str(), &data, &size);
    current->data.write(data, size);
    delete[] data;
}

chowstring BinaryArray::read_string(int pos, size_t size)
{
    DataStream stream(current->data);
    stream.seek(pos);
    chowstring v;
    stream.read_string(v, size);
    return v;
}

size_t BinaryArray::get_size()
{
    std::stringstream & oss = current->data;
    std::stringstream::pos_type current = oss.tellg();
    oss.seekg(0, std::ios::end);
    std::stringstream::pos_type offset = oss.tellg();
    oss.seekg(current);
    return (size_t)offset;
}
