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

#ifdef USE_ASSET_MANAGER

extern AAssetManager * global_asset_manager;
extern std::string internal_path;

enum ExtraFlags
{
    ANDROID_ASSET = BaseFile::CUSTOM << 0
};

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
    std::string new_path = internal_path + "/" + file_string;
    FILE * fp = fopen(new_path.c_str(), new_mode);
    if (fp != NULL) {
        handle = (void*)fp;
        return;
    }
    AAsset * asset = AAssetManager_open(global_asset_manager,
                                        file_string.c_str(),
                                        AASSET_MODE_UNKNOWN);
    if (asset == NULL) {
        flags |= CLOSED;
        return;
    }
    flags |= ANDROID_ASSET;
    handle = (void*)asset;
}

bool BaseFile::seek(size_t v, int origin)
{
    if (flags & ANDROID_ASSET) {
        return AAsset_seek(((AAsset*)handle), v, origin) == v;
    } else {
        return fseek((FILE*)handle, v, origin) == 0;
    }
}

size_t BaseFile::tell()
{
    if (flags & ANDROID_ASSET) {
        return AAsset_seek(((AAsset*)handle), 0, SEEK_CUR);
    } else {
        return ftell((FILE*)handle);
    }
}

size_t BaseFile::read(void * data, size_t size)
{
    if (flags & ANDROID_ASSET) {
        return AAsset_read(((AAsset*)handle), data, size);
    } else {
        return fread(data, 1, size, (FILE*)handle);
    }
}

size_t BaseFile::write(const void * data, size_t size)
{
    if (flags & ANDROID_ASSET) {
        return 0;
    } else {
        return fwrite(data, 1, size, (FILE*)handle);
    }
}

bool BaseFile::at_end()
{
    if (flags & ANDROID_ASSET) {
        return AAsset_getLength((AAsset*)handle) == 0;
    } else {
        FILE * fp = (FILE*)handle;
        int c = getc(fp);
        ungetc(c, fp);
        return c == EOF;
    }
}

void BaseFile::close()
{
    if (flags & CLOSED)
        return;
    flags |= CLOSED;
    if (flags & ANDROID_ASSET) {
        AAsset_close((AAsset*)handle);
    } else {
        fclose((FILE*)handle);
    }
}

#else

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
    SDL_RWops * new_handle = SDL_RWFromFile(file_string_c, new_mode);
    if (new_handle == NULL) {
        flags |= CLOSED;
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
}

void BaseFile::close()
{
    if (flags & CLOSED)
        return;
    SDL_RWops * rw = (SDL_RWops*)handle;
    SDL_RWclose(rw);
    flags |= CLOSED;
}

#endif
