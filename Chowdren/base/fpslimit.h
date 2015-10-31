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

#ifndef CHOWDREN_FPSLIMIT_H
#define CHOWDREN_FPSLIMIT_H

class FPSLimiter
{
public:
    int framerate;
    double current_framerate;
    double old_time;
    double next_update;
    double dt;

    FPSLimiter();
    void set(int value);
    void start();
    void finish();
    double normalize(double delta);
};

#endif // CHOWDREN_FPSLIMIT_H
