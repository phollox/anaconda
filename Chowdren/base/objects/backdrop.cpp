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

#include "objects/backdrop.h"
#include "collision.h"
#include "image.h"

// Backdrop

Backdrop::Backdrop(int x, int y, int type_id)
: FrameObject(x, y, type_id)
{
#if defined(CHOWDREN_IS_WIIU) || defined(CHOWDREN_EMULATE_WIIU)
    remote = CHOWDREN_HYBRID_TARGET;
#endif
}

Backdrop::~Backdrop()
{
    delete collision;
}

void Backdrop::draw()
{
#if defined(CHOWDREN_IS_WIIU) || defined(CHOWDREN_EMULATE_WIIU)
    int current_remote = platform_get_remote_value();
    if (remote == CHOWDREN_REMOTE_TARGET &&
        current_remote != CHOWDREN_HYBRID_TARGET)
        return;
#endif
    draw_image(image, x + image->hotspot_x, y + image->hotspot_y, blend_color);
}
