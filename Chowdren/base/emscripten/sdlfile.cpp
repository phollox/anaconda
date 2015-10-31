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

#include <SDL_rwops.h>

void BaseFile::open(const char * filename, const char * mode)
{
    flags = 0;
    const char * new_mode;
    switch (*mode) {
        case 'r':
            new_mode = "rb";
            break;
        case 'w':
            flags |= WRITE;
            new_mode = "wb";
            break;
    }

    std::string file_string = convert_path(filename);
    const char * file_string_c = file_string.c_str();
    std::cout << "Opening " << file_string_c << std::endl;
    SDL_RWops * new_handle = SDL_RWFromFile(file_string_c, new_mode);
    if (new_handle == NULL) {
        flags |= CLOSED;
        std::cout << "Could not open " << file_string_c << std::endl;
        return;
    }
    handle = (void*)new_handle;
}

bool BaseFile::seek(size_t v, int origin)
{
    SDL_RWops * rw = (SDL_RWops*)handle;
    return SDL_RWseek(rw, v, origin) >= 0;
}

size_t BaseFile::tell()
{
    SDL_RWops * rw = (SDL_RWops*)handle;
    return SDL_RWtell(rw);
}

size_t BaseFile::read(void * data, size_t size)
{
    SDL_RWops * rw = (SDL_RWops*)handle;
    return SDL_RWread(rw, data, 1, size);
}

size_t BaseFile::write(const void * data, size_t size)
{
    SDL_RWops * rw = (SDL_RWops*)handle;
    return SDL_RWwrite(rw, data, 1, size);
}

bool BaseFile::at_end()
{
    SDL_RWops * rw = (SDL_RWops*)handle;
    Sint64 pos = SDL_RWtell(rw);
    if (SDL_RWseek(rw, 0, RW_SEEK_END) == pos)
        return true;
    SDL_RWseek(rw, pos, RW_SEEK_SET);
    return false;
}

void BaseFile::close()
{
    if (flags & CLOSED)
        return;
    SDL_RWops * rw = (SDL_RWops*)handle;
    SDL_RWclose(rw);
    flags |= CLOSED;
}
