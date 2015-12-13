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

#ifndef CHOWDREN_STRINGREPLACE_H
#define CHOWDREN_STRINGREPLACE_H

#include "chowstring.h"
#include "types.h"
#include "frameobject.h"

class StringReplacement
{
public:
    chowstring from, to;

    StringReplacement(const chowstring & from, const chowstring & to)
    : from(from), to(to)
    {
    }
};

class StringReplace : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(StringReplace)

    vector<StringReplacement> replacements;

    StringReplace(int x, int y, int id);

    void add_replacement(const chowstring & from,
                         const chowstring & to);

    static chowstring replace(const chowstring & src,
                               const chowstring & from,
                               const chowstring & to);
    chowstring replace(const chowstring & src);
};

#endif // CHOWDREN_STRINGREPLACE_H
