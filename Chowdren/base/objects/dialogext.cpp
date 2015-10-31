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

#include "dialogext.h"
#include "stringcommon.h"
#include <iostream>
#include "platform.h"
#include "manager.h"

static std::string title;
static std::string text;

static std::string path;
static std::string default_path;

static Frame * file_dialog_frame = NULL;
static unsigned int file_dialog_loop_count = 0;

static Frame * dialog_frame = NULL;
static unsigned int dialog_loop_count = 0;

static DialogType dialog_type = DIALOG_OK;
static unsigned int dialog_id = -1;
static bool dialog_result;

void DialogObject::set_title(const std::string & value)
{
    title = value;
}

void DialogObject::set_text(const std::string & value)
{
    text = value;
}

void DialogObject::create(unsigned int id)
{
    dialog_id = id;
    dialog_result = platform_show_dialog(title, text, dialog_type);
    dialog_frame = manager.frame;
    dialog_loop_count = manager.frame->loop_count;
}

void DialogObject::set_ok()
{
    dialog_type = DIALOG_OK;
}

void DialogObject::set_yes_no()
{
    dialog_type = DIALOG_YESNO;
}

void DialogObject::set_modal()
{
    std::cout << "DialogObject::set_modal not implemented" << std::endl;
}

void DialogObject::set_topmost()
{
    std::cout << "DialogObject::set_topmost not implemented" << std::endl;
}

void DialogObject::set_filter(const std::string & filter, int count)
{
    std::cout << "DialogObject::set_filter not implemented: "
        << filter << " " << count << std::endl;
}

void DialogObject::set_default_filename(const std::string & filename)
{
    std::cout << "DialogObject::set_default_filename not implemented"
        << filename << std::endl;
    default_path = filename;
}

void DialogObject::set_default_extension(const std::string & ext)
{
    std::cout << "DialogObject::set_default_extension not implemented"
        << ext << std::endl;
}

void DialogObject::set_default_directory(const std::string & dir)
{
    std::cout << "DialogObject::set_default_directory not implemented"
        << dir << std::endl;
}

void DialogObject::open_load_selector(const std::string & dir)
{
    vector<std::string> names;
    file_dialog_frame = NULL;
    path.clear();
    if (!platform_file_open_dialog(title, "", dir, false, names))
        return;
    file_dialog_loop_count = manager.frame->loop_count;
    file_dialog_frame = manager.frame;
    path = names[0];
}

void DialogObject::open_save_selector(const std::string & dir)
{
    path.clear();
    if (!platform_file_save_dialog(title, "", dir, path))
        return;
    file_dialog_loop_count = manager.frame->loop_count;
    file_dialog_frame = manager.frame;
}

bool DialogObject::is_file_success()
{
    if (manager.frame != file_dialog_frame)
        return false;
    if (manager.frame->loop_count > file_dialog_loop_count) {
        file_dialog_frame = NULL;
        return false;
    }
    return true;
}

bool DialogObject::is_success(unsigned int id)
{
    if (dialog_id != id)
        return false;
    if (manager.frame != dialog_frame)
        return false;
    if (manager.frame->loop_count > dialog_loop_count) {
        dialog_frame = NULL;
        return false;
    }
    return dialog_result;
}

bool DialogObject::is_failure(unsigned int id)
{
    if (dialog_id != id)
        return false;
    if (manager.frame != file_dialog_frame)
        return false;
    if (manager.frame->loop_count > file_dialog_loop_count) {
        file_dialog_frame = NULL;
        return false;
    }
    return !dialog_result;
}

const std::string & DialogObject::get_path()
{
    return path;
}
