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

#include <string>
#include <iostream>
#include "platform.h"
#include <SDL_rwops.h>

// void platform_walk_folder(const std::string & in_path,
//                           FolderCallback & callback)
// {
// }

// size_t platform_get_file_size(const char * filename)
// {
//     std::string path = convert_path(filename);
//     SDL_RWops * rw = SDL_RWFromFile(path.c_str(), "rb");
//     if (rw == NULL)
//         return 0;
//     size_t size = SDL_RWsize(rw);
//     SDL_RWclose(rw);
//     return size;
// }

// bool platform_is_directory(const std::string & value)
// {
//     return false;
// }

// bool platform_is_file(const std::string & value)
// {
//     std::string path = convert_path(value);
//     SDL_RWops * rw = SDL_RWFromFile(path.c_str(), "rb");
//     if (rw != NULL) {
//         SDL_RWclose(rw);
//         return true;
//     }
//     return false;
// }

// bool platform_path_exists(const std::string & value)
// {
//     return platform_is_file(value);
// }

// void platform_create_directories(const std::string & value)
// {
// }

// const std::string & platform_get_appdata_dir()
// {
//     static std::string dir(".");
//     return dir;
// }

#include "../desktop/platform.cpp"
