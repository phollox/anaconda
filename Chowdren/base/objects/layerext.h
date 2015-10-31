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

#ifndef CHOWDREN_LAYEREXT_H
#define CHOWDREN_LAYEREXT_H

#include "frameobject.h"
#include "color.h"
#include <string>

class LayerObject : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(LayerObject)

    int current_layer;
    static int sort_index;
    static bool sort_reverse;
    static double def;

    LayerObject(int x, int y, int type_id);
    void set_layer(int value);
    void hide_layer(int index);
    void show_layer(int index);
    void set_x(int index, int x);
    void set_y(int index, int y);
    int get_x(int index);
    int get_y(int index);
    void set_position(int index, int x, int y);
    void set_alpha_coefficient(int index, int alpha);
    static double get_alterable(const FrameObject & instance);
    static bool sort_func_inc(const FrameObject & a, const FrameObject & b);
    static bool sort_func_dec(const FrameObject & a, const FrameObject & b);
    void sort_alt_decreasing(int index, double def);
    void set_rgb(int index, Color color);
};

#endif // CHOWDREN_LAYEREXT_H
