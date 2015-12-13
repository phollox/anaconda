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

#include "objects/stringreplace.h"
#include "stringcommon.h"

inline void ireplace_substring(chowstring & str,
                               const chowstring & from,
                               const chowstring & to)
{
    if (from.empty())
        return;
    boost::algorithm::ireplace_all(str, from, to);
}

StringReplace::StringReplace(int x, int y, int id)
: FrameObject(x, y, id)
{
}

void StringReplace::add_replacement(const chowstring & from,
                                    const chowstring & to)
{
    replacements.push_back(StringReplacement(from, to));
}

chowstring StringReplace::replace(const chowstring & src,
                                   const chowstring & from,
                                   const chowstring & to)
{
    chowstring ret(src);
    ireplace_substring(ret, from, to);
    return ret;
}

chowstring StringReplace::replace(const chowstring & src)
{
    chowstring ret(src);
    vector<StringReplacement>::const_iterator it;
    for (it = replacements.begin(); it != replacements.end(); it++) {
        const StringReplacement & r = *it;
        ireplace_substring(ret, r.from, r.to);
    }
    return ret;
}
