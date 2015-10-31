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

#include "transition.h"
#include "manager.h"
#include "render.h"

void Transition::draw(int fade_type, float fade_value, Color color)
{
    switch (fade_type) {
        case FADE:
            color.set_alpha(int(fade_value * 255));
            Render::draw_quad(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, color);
            break;
        case DOOR:
            int w = (WINDOW_WIDTH / 2.0f) * fade_value;
            Render::draw_quad(0, 0, w, WINDOW_HEIGHT, color);
            Render::draw_quad(WINDOW_WIDTH - w, 0,
                              WINDOW_WIDTH, WINDOW_HEIGHT, color);
            break;
    }
}