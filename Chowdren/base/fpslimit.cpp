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

#include "platform.h"
#include <iostream>
#include "fpslimit.h"
#include <algorithm>

FPSLimiter::FPSLimiter()
: framerate(-1), dt(0.0), current_framerate(0.0)
{
}

void FPSLimiter::set(int value)
{
    framerate = value;
}

double FPSLimiter::normalize(double delta)
{
    if (delta > 1.0)
        return 1.0 / framerate;
    return delta;
}

void FPSLimiter::start()
{
    old_time = platform_get_time();
    next_update = old_time;
}

void FPSLimiter::finish()
{
    double current_time = platform_get_time();

#if defined(CHOWDREN_IS_DESKTOP) || defined(CHOWDREN_IS_ANDROID)
    if (framerate < 100) {
        double t = normalize(next_update - current_time);
        platform_sleep(t);
    }
    next_update = std::max(current_time, next_update) + 1.0 / framerate;
#endif

    dt = normalize(current_time - old_time);
    old_time = current_time;
    if (dt < 0.0)
        dt = 0.001;
    current_framerate = 1.0 / dt;
}
