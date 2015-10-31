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

#include "objects/layerext.h"
#include "chowconfig.h"

// LayerObject

int LayerObject::sort_index;
bool LayerObject::sort_reverse;
double LayerObject::def;

LayerObject::LayerObject(int x, int y, int type_id)
: FrameObject(x, y, type_id), current_layer(-1)
{
}

void LayerObject::set_layer(int value)
{
    current_layer = value;
}

void LayerObject::hide_layer(int index)
{
    frame->layers[index].hide();
}

void LayerObject::show_layer(int index)
{
    frame->layers[index].show();
}

void LayerObject::set_position(int index, int x, int y)
{
    frame->layers[index].set_position(x, y);
}

void LayerObject::set_x(int index, int x)
{
    Layer & layer = frame->layers[index];
    layer.set_position(x, layer.y);
}

void LayerObject::set_y(int index, int y)
{
    Layer & layer = frame->layers[index];
    layer.set_position(layer.x, y);
}

int LayerObject::get_x(int index)
{
    return frame->layers[index].x;
}

int LayerObject::get_y(int index)
{
    return frame->layers[index].y;
}

void LayerObject::set_alpha_coefficient(int index, int alpha)
{
    Layer * layer = &frame->layers[index];
    FlatObjectList::const_iterator it;
    for (it = layer->background_instances.begin();
         it != layer->background_instances.end(); ++it) {
        FrameObject * obj = *it;
        obj->blend_color.set_alpha_coefficient(alpha);
    }
    layer->blend_color.set_alpha_coefficient(alpha);
}

double LayerObject::get_alterable(const FrameObject & instance)
{
    if (instance.alterables == NULL)
        return def;
    return instance.alterables->values.get(sort_index);
}

bool LayerObject::sort_func_dec(const FrameObject & a, const FrameObject & b)
{
    double value1 = get_alterable(a);
    double value2 = get_alterable(b);
    return value1 < value2;
}

bool LayerObject::sort_func_inc(const FrameObject & a, const FrameObject & b)
{
    double value1 = get_alterable(a);
    double value2 = get_alterable(b);
    return value1 > value2;
}

void LayerObject::sort_alt_decreasing(int index, double def)
{
    sort_index = index;
    this->def = def;
    if (current_layer == -1)
        current_layer = layer->index;
    Layer * layer = &frame->layers[current_layer];
    layer->instances.sort(sort_func_dec);
    layer->reset_depth();
}

void LayerObject::set_rgb(int index, Color color)
{
    frame->layers[index].blend_color = color;
}
