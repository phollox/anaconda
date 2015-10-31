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

#ifndef CHOWDREN_ASSARRAY_H
#define CHOWDREN_ASSARRAY_H

#include "frameobject.h"
#include "objects/blowfish.h"
#include "types.h"

class AssociateArrayItem
{
public:
    int value;
    std::string string;

    AssociateArrayItem()
    : value(0)
    {
    }
};

// #include <boost/container/flat_map.hpp>
// typedef boost::container::flat_map<std::string, AssociateArrayItem> ArrayMap;

//// XXX is this faster?
typedef hash_map<std::string, AssociateArrayItem> ArrayMap;

class ArrayAddress
{
public:
    ArrayMap::const_iterator it;
    bool null;

    ArrayAddress(const ArrayMap::const_iterator & it)
    : it(it), null(false)
    {
    }

    ArrayAddress()
    : null(true)
    {
    }
};

class AssociateArray : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(AssociateArray)

    Blowfish cipher;

    ArrayMap * map;
    static ArrayMap global_map;

    AssociateArrayItem * store[CHOWDREN_ASSARRAY_STORE];

    AssociateArray(int x, int y, int type_id);
    ~AssociateArray();
    void init_global();
    void load_encrypted(const std::string & filename, int method);
    void load_data(const std::string & data, int method);
    void set_value(const std::string & key, int value);
    void add_value(const std::string & key, int value);
    void sub_value(const std::string & key, int value);
    void set_string(const std::string & key, const std::string & value);
    int get_value(const std::string & key);
    const std::string & get_string(const std::string & key);
    void set_key(const std::string & key);
    void remove_key(const std::string & key);
    bool has_key(const std::string & key);
    bool count_prefix(const std::string & key, int count);
    int count_prefix(const std::string & key);
    void clear();
    ArrayAddress get_first();
    ArrayAddress get_prefix(const std::string & prefix, int index,
                            ArrayAddress start);
    const std::string & get_key(ArrayAddress addr);
    void save(const std::string & filename, int method);
    void save_encrypted(const std::string & filename, int method);

    // set/get with store
    int get_value(int store, const std::string & key);
    void set_value(int store, const std::string & key, int value);
    void add_value(int store, const std::string & key, int value);
    void sub_value(int store, const std::string & key, int value);
    const std::string & get_string(int store, const std::string & key);
    void set_string(int store, const std::string & key,
                    const std::string & value);
    bool has_key(int store, const std::string & key);
};

extern FrameObject * default_assarray_instance;

#endif // CHOWDREN_ASSARRAY_H
