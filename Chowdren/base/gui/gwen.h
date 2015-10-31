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

#ifndef CHOWDREN_GWEN_H
#define CHOWDREN_GWEN_H

#include "Gwen/Gwen.h"
#include "Gwen/Skins/Simple.h"
#include "Gwen/Renderers/Chowdren.h"
#include <string>

class GwenData
{
public:
    int m_x, m_y;

    std::string title;

    Gwen::Controls::Canvas * canvas;
    Gwen::Controls::Base * frame_base;

    GwenData();
    ~GwenData();

    // Handle animations, input etc.
    void update();

    // Called from every extension to render itself
    void render(Gwen::Controls::Base* control);
};

#endif // CHOWDREN_MANAGER_H
