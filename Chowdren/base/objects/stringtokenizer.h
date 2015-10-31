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

#ifndef CHOWDREN_STRINGTOKENIZER_H
#define CHOWDREN_STRINGTOKENIZER_H

#include "frameobject.h"
#include <string>
#include "types.h"

class StringTokenizer : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(StringTokenizer)

    vector<std::string> elements;

    StringTokenizer(int x, int y, int type_id);
    void split(const std::string & text, const std::string & delims);
    const std::string & get(int index);

    int get_count()
    {
        return elements.size();
    }
};

#endif // CHOWDREN_STRINGTOKENIZER_H
