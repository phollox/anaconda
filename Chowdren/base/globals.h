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

#ifndef GLOBALS_H
#define GLOBALS_H

#include "types.h"
#include "stringcommon.h"
#include <string>
#include "dynnum.h"

class GlobalValues
{
public:
    vector<DynamicNumber> values;

    GlobalValues()
    {
    }

    DynamicNumber get(size_t index)
    {
        if (index >= values.size())
            return 0;
        return values[index];
    }

    int get_int(size_t index)
    {
        if (index >= values.size())
            return 0;
        return int(values[index]);
    }

    void set(size_t index, DynamicNumber value)
    {
        if (index >= values.size()) {
            values.resize(index + 1);
        }
        values[index] = value;
    }

    void add(size_t index, DynamicNumber value)
    {
        set(index, get(index) + value);
    }

    void sub(size_t index, DynamicNumber value)
    {
        set(index, get(index) - value);
    }
};

class GlobalStrings
{
public:
    vector<std::string> values;

    GlobalStrings()
    {
    }

    const std::string & get(size_t index)
    {
        if (index >= values.size()) {
            return empty_string;
        }
        return values[index];
    }

    void set(size_t index, const std::string & value)
    {
        if (index >= values.size())
            values.resize(index + 1);
        values[index] = value;
    }
};

#endif // GLOBALS_H
