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

#include "objects/counter.h"
#include "collision.h"
#include <iomanip>
#include "common.h"

// Counter

Counter::Counter(int x, int y, int type_id)
: FrameObject(x, y, type_id), flash_interval(0.0f), zero_pad(0)
{
}

Counter::~Counter()
{
    delete collision;
}

void Counter::calculate_box()
{
    if (type == IMAGE_COUNTER) {
        width = 0;
        height = 0;
        for (chowstring::const_iterator it = cached_string.begin();
             it != cached_string.end(); ++it) {
            Image * image = get_image(it[0]);
            width += image->width;
            height = std::max<int>(image->height, height);
        }
        ((OffsetInstanceBox*)collision)->set_offset(-width, -height);
    } else if (type == ANIMATION_COUNTER) {
        Image * image = get_image();
        width = image->width;
        height = image->height;
        ((OffsetInstanceBox*)collision)->update_aabb();
    }
}

Image * Counter::get_image()
{
    if (maximum <= minimum)
        return images[0];
    int max_index = image_count - 1;
    int i = (((value - minimum) * max_index) / (maximum - minimum));
    Image * image = images[i];
    image->load();
    return image;
}

Image * Counter::get_image(char c)
{
    Image * image;
    switch (c) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            image = images[c - '0'];
            break;
        case '-':
            image = images[10];
            break;
        case '+':
            image = images[11];
            break;
        case '.':
            image = images[12];
            break;
        case 'e':
            image = images[13];
            break;
        default:
            return NULL;
    }
    image->load();
    return image;
}

void Counter::add(double value)
{
    set(this->value + value);
}

void Counter::subtract(double value)
{
    set(this->value - value);
}

void Counter::set(double value)
{
    value = std::max<double>(std::min<double>(value, maximum), minimum);
    this->value = value;

    if (type == HIDDEN_COUNTER)
        return;

    if (collision == NULL)
        collision = new OffsetInstanceBox(this);

    if (type == IMAGE_COUNTER) {
        if (zero_pad > 0) {
            std::ostringstream str;
            str << std::setw(zero_pad) << std::setfill('0') << value;
        } else {
            cached_string = number_to_string(value);
        }
        calculate_box();
    } else if (type == ANIMATION_COUNTER) {
        calculate_box();
    }
}

void Counter::set_max(int value)
{
    maximum = value;
    set(this->value);
}

void Counter::set_min(int value)
{
    minimum = value;
    set(this->value);
}

void Counter::update()
{
    update_flash(flash_interval, flash_time);
}

void Counter::flash(float value)
{
    flash_interval = value;
    flash_time = 0.0f;
}

void Counter::draw()
{
    if (type == HIDDEN_COUNTER)
        return;

    if (type == IMAGE_COUNTER) {
        double current_x = x;
        const char * start = cached_string.data();
        const char * end = start + cached_string.size();
        for (const char * pos = end - 1; pos >= start; pos--) {
            Image * image = get_image(*pos);
            if (image == NULL)
                continue;
            image->draw(current_x + image->hotspot_x - image->width,
                        y + image->hotspot_y - image->height, blend_color);
            current_x -= image->width;
        }
    } else if (type == VERTICAL_UP_COUNTER) {
        int draw_height = ((value - minimum) * height) /
                          (maximum - minimum);

        int x1 = x;
        int y2 = y + height;
        int y1 = y2 - draw_height;
        int x2 = x + width;

        draw_gradient(x1, y1, x2, y2, gradient_type, color1, color2,
                      blend_color.a);
    } else if (type == HORIZONTAL_LEFT_COUNTER) {
        int draw_width = ((value - minimum) * width) /
                          (maximum - minimum);

        int x1 = x;
        int y1 = y;
        int x2 = x + draw_width;
        int y2 = y + height;

        draw_gradient(x1, y1, x2, y2, gradient_type, color1, color2,
                      blend_color.a);
    } else if (type == ANIMATION_COUNTER) {
        Image * image = get_image();
        image->draw(x + image->hotspot_x, y + image->hotspot_y, blend_color);
    }
}
