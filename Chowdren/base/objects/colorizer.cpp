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

#include "colorizer.h"

ColorizerObject::ColorizerObject(int x, int y, int id)
: FrameObject(x, y, id), r(1.0f), g(1.0f), b(1.0f)
{

}

inline float round_color(float v)
{
    return int(v * 256.0f) / 256.0f;
}

void ColorizerObject::set_red(float v)
{
    r = round_color(v);
}

void ColorizerObject::set_green(float v)
{
    g = round_color(v);
}

void ColorizerObject::set_blue(float v)
{
    b = round_color(v);
}
