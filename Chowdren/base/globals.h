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
#include "chowstring.h"
#include "dynnum.h"
#include "chowconfig.h"

#ifndef NDEBUG
#include <iostream>
#endif

#ifndef CHOWDREN_MAX_GLOBAL_VALUES
#define CHOWDREN_MAX_GLOBAL_VALUES 256
#endif

#ifndef CHOWDREN_MAX_GLOBAL_STRINGS
#define CHOWDREN_MAX_GLOBAL_STRINGS 256
#endif

class GlobalValues
{
public:
    DynamicNumber values[CHOWDREN_MAX_GLOBAL_VALUES];

    GlobalValues()
    {
        for (int i = 0; i < CHOWDREN_MAX_GLOBAL_VALUES; ++i)
            values[i] = 0;
    }

    DynamicNumber get(size_t index)
    {
#ifndef NDEBUG
        if (index >= CHOWDREN_MAX_GLOBAL_VALUES) {
            std::cout << "Global value out of bounds: " << index << std::endl;
            return 0;
        }
#endif
        return values[index];
    }

    int get_int(size_t index)
    {
#ifndef NDEBUG
        if (index >= CHOWDREN_MAX_GLOBAL_VALUES) {
            std::cout << "Global value out of bounds: " << index << std::endl;
            return 0;
        }
#endif
        return int(values[index]);
    }

    void set(size_t index, DynamicNumber value)
    {
#ifndef NDEBUG
        if (index >= CHOWDREN_MAX_GLOBAL_VALUES) {
            std::cout << "Global value out of bounds: " << index << std::endl;
			return;
        }
#endif
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
    chowstring values[CHOWDREN_MAX_GLOBAL_STRINGS];

    GlobalStrings()
    {
    }

    const chowstring & get(size_t index)
    {
#ifndef NDEBUG
        if (index >= CHOWDREN_MAX_GLOBAL_STRINGS) {
            std::cout << "Global string out of bounds: " << index << std::endl;
            return empty_string;
        }
#endif
        return values[index];
    }

    void set(size_t index, const chowstring & value)
    {
#ifndef NDEBUG
        if (index >= CHOWDREN_MAX_GLOBAL_STRINGS) {
            std::cout << "Global string out of bounds: " << index << std::endl;
			return;
        }
#endif
        values[index] = value;
    }
};

#endif // GLOBALS_H
