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

#include "objects/listext.h"
#include "chowconfig.h"

// ListObject

ListObject::ListObject(int x, int y, int type_id)
: FrameObject(x, y, type_id), list_flags(0)
{

}

void ListObject::load_file(const std::string & name)
{
    std::string data;
    if (!read_file(name.c_str(), data))
        return;
    StringStream ss(data);
    std::string line;
    while (!ss.at_end()) {
        ss.read_line(line);
        add_line(line);
    }

    if (list_flags & SORT_LIST)
        sort();
}

void ListObject::delete_line(int line)
{
    line += index_offset;
    if (line < 0 || line >= int(lines.size()))
        return;
    lines.erase(lines.begin() + line);
}

void ListObject::clear()
{
    lines.clear();
}

void ListObject::add_line(const std::string & value)
{
    lines.push_back(value);

    if (list_flags & SORT_LIST)
        sort();
}

const std::string & ListObject::get_line(int i)
{
    i += index_offset;
    if (i < 0 || i >= int(lines.size()))
        return empty_string;
    return lines[i];
}

const std::string & ListObject::get_current_line()
{
    return get_line(current_line);
}

int ListObject::find_string(const std::string & text, int flag)
{
#ifndef NDEBUG
    if (flag != -1)
        std::cout << "Unsupported find_string: " << flag << std::endl;
#endif
    for (int i = 0; i < int(lines.size()); ++i) {
        if (starts_with(lines[i], text))
            return i - index_offset;
    }
    return -1;
}

int ListObject::find_string_exact(const std::string & text, int flag)
{
#ifndef NDEBUG
    if (flag != -1)
        std::cout << "Unsupported find_string_exact: " << flag << std::endl;
#endif
    for (int i = 0; i < int(lines.size()); ++i) {
        if (lines[i] == text)
            return i - index_offset;
    }
    return -1;
}

void ListObject::set_line(int line, const std::string & value)
{
    line += index_offset;
    if (line < 0 || line >= int(lines.size()))
        return;
    lines[line] = value;

    if (list_flags & SORT_LIST)
        sort();
}

int ListObject::get_count()
{
    return int(lines.size());
}

bool ListObject::get_focus()
{
    // std::cout << "List: get_focus not implemented" << std::endl;
    return false;
}

void ListObject::disable_focus()
{
    // std::cout << "List: disable_focus not implemented" << std::endl;
}

const unsigned int collation_table[256] =
{
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02010111, 0x02020111, 0x02030111,
    0x02040111, 0x02050111, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x02090111, 0x024b0111, 0x02700111, 0x02a90111,
    0x09e00111, 0x02aa0111, 0x02a70111, 0x02690111,
    0x027a0111, 0x027b0111, 0x02a20111, 0x039f0111,
    0x022d0111, 0x02210111, 0x02550111, 0x02a40111,
    0x0a0b0111, 0x0a0c0111, 0x0a0d0111, 0x0a0e0111,
    0x0a0f0111, 0x0a100111, 0x0a110111, 0x0a120111,
    0x0a130111, 0x0a140111, 0x02370111, 0x02350111,
    0x03a30111, 0x03a40111, 0x03a50111, 0x024e0111,
    0x02a10111, 0x0a150151, 0x0a290141, 0x0a3d0151,
    0x0a490151, 0x0a650151, 0x0a910151, 0x0a990151,
    0x0ab90151, 0x0ad30161, 0x0ae70141, 0x0af70141,
    0x0b030161, 0x0b2b0151, 0x0b330151, 0x0b4b0161,
    0x0b670141, 0x0b730141, 0x0b7f0141, 0x0ba70151,
    0x0bbf0151, 0x0bd70141, 0x0bef0151, 0x0bfb0141,
    0x0c030151, 0x0c070141, 0x0c130141, 0x027c0111,
    0x02a60111, 0x027d0111, 0x020f0111, 0x021b0111,
    0x020c0111, 0x0a150111, 0x0a290111, 0x0a3d0111,
    0x0a490111, 0x0a650111, 0x0a910111, 0x0a990111,
    0x0ab90111, 0x0ad30111, 0x0ae70111, 0x0af70111,
    0x0b030111, 0x0b2b0111, 0x0b330111, 0x0b4b0111,
    0x0b670111, 0x0b730111, 0x0b7f0111, 0x0ba70111,
    0x0bbf0111, 0x0bd70111, 0x0bef0111, 0x0bfb0111,
    0x0c030111, 0x0c070111, 0x0c130111, 0x027e0111,
    0x03a70111, 0x027f0111, 0x03aa0111, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02060111, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x02090141, 0x024c0111, 0x09df0111, 0x09e10111,
    0x09de0111, 0x09e20111, 0x03a80111, 0x029c0111,
    0x02140111, 0x029f0111, 0x0a150181, 0x02780111,
    0x03a60111, 0x02200111, 0x02a00111, 0x02100111,
    0x030a0111, 0x03a00111, 0x0a0d0151, 0x0a0e0151,
    0x020d0111, 0x0c9f0121, 0x029d0111, 0x025f0111,
    0x02190111, 0x0a0c0151, 0x0b4b01a1, 0x02790111,
    0x0a0c0171, 0x0a0c0171, 0x0a0e0171, 0x024f0111,
    0x0a150151, 0x0a150151, 0x0a150151, 0x0a150151,
    0x0a150151, 0x0a150151, 0x0a190121, 0x0a3d0151,
    0x0a650151, 0x0a650151, 0x0a650151, 0x0a650151,
    0x0ad30161, 0x0ad30161, 0x0ad30161, 0x0ad30161,
    0x0a5d0121, 0x0b330151, 0x0b4b0161, 0x0b4b0161,
    0x0b4b0161, 0x0b4b0161, 0x0b4b0161, 0x03a20111,
    0x0b530121, 0x0bd70141, 0x0bd70141, 0x0bd70141,
    0x0bd70141, 0x0c070141, 0x0c3b0121, 0x0ba70131,
    0x0a150111, 0x0a150111, 0x0a150111, 0x0a150111,
    0x0a150111, 0x0a150111, 0x0a190111, 0x0a3d0111,
    0x0a650111, 0x0a650111, 0x0a650111, 0x0a650111,
    0x0ad30111, 0x0ad30111, 0x0ad30111, 0x0ad30111,
    0x0a5d0111, 0x0b330111, 0x0b4b0111, 0x0b4b0111,
    0x0b4b0111, 0x0b4b0111, 0x0b4b0111, 0x03a10111,
    0x0b530111, 0x0bd70111, 0x0bd70111, 0x0bd70111,
    0x0bd70111, 0x0c070111, 0x0c3b0111, 0x0c070111
};

inline int compare_weights2(const char * str1, int len1,
                                   const char * str2, int len2)
{
    unsigned int ce1, ce2;
    int ret;
    while (len1 > 0 && len2 > 0) {
        ce1 = collation_table[*str1];
        ce2 = collation_table[*str2];
        ret = ((ce1 >> 8) & 0xff) - ((ce2 >> 8) & 0xff);
        if (ret)
            return ret;

        str1++;
        str2++;
        len1--;
        len2--;
    }
    return len1 - len2;
}


inline int compare_weights1(const char * str1, int len1,
                           const char * str2, int len2)
{
    int ret;
    unsigned int ce1, ce2;

    while (len1 > 0 && len2 > 0) {
        if (*str1 == '-' || *str1 == '\'') {
            if (*str2 != '-' && *str2 != '\'') {
                str1++;
                len1--;
                continue;
            }
        } else if (*str2 == '-' || *str2 == '\'') {
            str2++;
            len2--;
            continue;
        }

        ce1 = collation_table[*str1];
        ce2 = collation_table[*str2];
        ret = (ce1 >> 16) - (ce2 >> 16);

        if (ret)
            return ret;

        str1++;
        str2++;
        len1--;
        len2--;
    }
    return len1 - len2;
}

inline bool list_sort(const std::string & a, const std::string & b)
{
    int len1 = int(a.size());
    int len2 = int(b.size());
    int ret = compare_weights1(&a[0], len1, &b[0], len2);
    if (!ret)
        ret = compare_weights2(&a[0], len1, &b[0], len2);
    return ret < 0;
}

void ListObject::sort()
{
    std::sort(lines.begin(), lines.end(), list_sort);
}

void ListObject::set_current_line(int index)
{
    int i = index + index_offset;
    if (i < 0 || i >= int(lines.size()))
        return;
    current_line = index;
}