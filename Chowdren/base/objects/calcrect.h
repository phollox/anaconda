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

#ifndef CHOWDREN_CALCRECT_H
#define CHOWDREN_CALCRECT_H

#include <string>

class CalcRect
{
public:
    static void set_font(const std::string & text, int size, int style);
    static void set_text(const std::string & text);
    static void set_max_width(int width);
    static int get_width();
    static int get_height();
};

#endif // CHOWDREN_CALCRECT_H
