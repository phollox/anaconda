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

#ifndef CHOWDREN_COUNTER_H
#define CHOWDREN_COUNTER_H

#include "frameobject.h"
#include "chowconfig.h"
#include <string>
#include "color.h"
#include "image.h"

#define HIDDEN_COUNTER 0
#define IMAGE_COUNTER 1
#define VERTICAL_UP_COUNTER 2
#define ANIMATION_COUNTER 3
#define HORIZONTAL_LEFT_COUNTER 4

class Counter : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(Counter)

    struct SavedCounter
    {
        bool init;
        double value;

        SavedCounter()
        : init(false)
        {
        }
    };

    int image_count;
    Image ** images;
    double value;
    int minimum, maximum;
    std::string cached_string;
    int type;
    float flash_time, flash_interval;
    int gradient_type;
    Color color1;
    Color color2;
    int zero_pad;

    Counter(int x, int y, int type_id);
    ~Counter();
    Image * get_image(char c);
    Image * get_image();
    void add(double value);
    void subtract(double value);
    void set_max(int value);
    void set_min(int value);
    void set(double value);
    void draw();
    void calculate_box();
    void update();
    void flash(float value);

    int get_int()
    {
        return int(value);
    }
};

#endif // CHOWDREN_COUNTER_H
