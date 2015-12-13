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

#ifndef CHOWDREN_STRINGCOMMON_H
#define CHOWDREN_STRINGCOMMON_H

#include "chowstring.h"
#include <sstream>
#include <ctype.h>
#include "types.h"
#include <algorithm>
#include "dynnum.h"
#include <boost/algorithm/string/replace.hpp>

int fast_atoi(const chowstring & value);
double fast_atof(const char * p, const char * end);
chowstring fast_itoa(int value);
chowstring fast_lltoa(long long value);
chowstring fast_dtoa(double value);

extern chowstring empty_string;
extern chowstring newline_character;
extern chowstring unix_newline_character;

inline double string_to_double(const chowstring & in)
{
    if (in.empty())
        return 0.0;
    const char * start = in.data();
    const char * end = start + in.size();
    return fast_atof(start, end);
}

inline int string_to_int(const chowstring & in)
{
    return fast_atoi(in);
}

inline double string_to_number(const chowstring & in)
{
#ifdef CHOWDREN_USE_DYNAMIC_NUMBER
    double ret = string_to_double(in);
    int int_ret = int(ret);
    if (int_ret == ret)
        return DynamicNumber(int_ret);
    return DynamicNumber(ret);
#else
    return string_to_double(in);
#endif
}

inline const chowstring & number_to_string(const chowstring & value)
{
    return value;
}

inline chowstring number_to_string(double value)
{
    return fast_dtoa(value);
}

inline chowstring number_to_string(int value)
{
    return fast_itoa(value);
}

inline chowstring number_to_string(unsigned int value)
{
    return fast_itoa(value);
}

inline chowstring number_to_string(long long value)
{
    return fast_lltoa(value);
}

inline chowstring number_to_string(uint64_t value)
{
    return fast_lltoa(value);
}

inline void to_lower(chowstring & str)
{
    std::transform(str.begin(), str.end(), str.begin(),
                   static_cast<int(*)(int)>(tolower));
}

inline void replace_substring(chowstring & str,
                              const chowstring & from,
                              const chowstring & to)
{
    if (from.empty())
        return;
#ifdef USE_CHOWSTRING
    if (str.empty())
        return;
    str.make_unique();
    unsigned int from_size = from.size();
    unsigned int to_size = to.size();
#else
    boost::algorithm::replace_all(str, from, to);
#endif
}

inline void split_string(const chowstring & s, char delim,
                         vector<chowstring> & elems)
{
    chowstring item;
    unsigned int pos = 0;
    while (pos < s.size()) {
        unsigned int end = pos;
        while (end < s.size() && s[end] != delim)
            end++;
        elems.push_back(s.substr(pos, end - pos));
        pos = end + 1;
    }
}

inline void split_string(const chowstring & str, const chowstring & delims,
                         vector<chowstring> & elems)
{
    unsigned int pos = 0;
    while (pos < str.size()) {
        unsigned int end = pos;
        while (end < str.size()) {
            char c = str[end];
            for (unsigned int i = 0; i < delims.size(); ++i) {
                if (delims[i] == c) {
                    goto quit;
                }
            }
            end++;
        }
    quit:
        elems.push_back(str.substr(pos, end - pos));
        pos = end + 1;
    }
}

inline bool ends_with(const chowstring & str, const chowstring & suffix)
{
    if (str.size() < suffix.size())
        return false;
    return str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

inline bool starts_with(const chowstring & str, const chowstring & prefix)
{
    if (str.size() < prefix.size())
        return false;
    return str.compare(0, prefix.size(), prefix) == 0;
}

#endif // CHOWDREN_STRINGCOMMON_H
