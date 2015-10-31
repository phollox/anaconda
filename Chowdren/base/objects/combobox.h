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

#ifndef CHOWDREN_COMBOBOX_H
#define CHOWDREN_COMBOBOX_H

#include "frameobject.h"
#include "common.h"

#include "Gwen/Gwen.h"
#include "Gwen/Controls/ComboBox.h"
#include "gui/gwen.h"


class ComboBox : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(ComboBox)

    ComboBox(int x, int y, int type_id);
    ~ComboBox();

    Gwen::Controls::ComboBox * combo_box;
    int old_index;
    int selection_changed;
    int index_offset;

    void update();
    void draw();
    int get_current_line_number();
    std::string get_current_line();
    std::string get_line(int index);
    void set_current_line(int index);
    void add_line(const std::string line);
    void highlight();
    void dehighlight();
    void lose_focus();
    void reset();
    bool is_list_dropped();
    bool is_selection_changed();
    int find_string_exact(const std::string & text, int flag);
    void init_control();
};

#endif // CHOWDREN_COMBOBOX_H
