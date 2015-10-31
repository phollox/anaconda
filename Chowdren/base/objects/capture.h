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

#ifndef CHOWDREN_CAPTURE_H
#define CHOWDREN_CAPTURE_H

#include <string>
#include "image.h"

class CaptureObject
{
public:
    static std::string filename;
    static Image image;
    static bool capture;

    static void set_filename(const std::string & filename);
    static void capture_frame();
    static void capture_window();
    static void set_origin(int x, int y);
    static void set_size(int w, int h);
    static void on_capture();
};

#endif // CHOWDREN_CAPTURE_H
