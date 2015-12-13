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

#ifndef CHOWDREN_STRINGPARSER_H
#define CHOWDREN_STRINGPARSER_H

#include "chowstring.h"
#include "types.h"
#include "frameobject.h"

class StringParser : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(StringParser)

    vector<chowstring> elements;
    chowstring delimiters;
    chowstring value;
    bool has_split;

    StringParser(int x, int y, int id);
    void split();
    void load(const chowstring & filename);
    void set(const chowstring & value);
    void add_delimiter(const chowstring & delim);
    void reset_delimiters();
    const chowstring & get_element(int index);
    chowstring set_element(const chowstring & value, int index);
    const chowstring & get_last_element();
    chowstring replace(const chowstring & from, const chowstring & to);
    chowstring remove(const chowstring & sub);
    int get_count();
    chowstring get_md5();
};

#endif // CHOWDREN_STRINGPARSER_H
