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

#ifndef CHOWDREN_FILEIO_H
#define CHOWDREN_FILEIO_H

#include "chowstring.h"
#include <stdio.h>

// #define CHOWDREN_FILE_BUFFERING

class BaseFile
{
public:
    void * handle;
    int flags;

    enum FileFlags
    {
        CLOSED = 1 << 0,
        WRITE = 1 << 1,
        CUSTOM = 1 << 2
    };

    BaseFile();
    BaseFile(const char * filename, const char * mode);
    ~BaseFile();
    void open(const char * filename, const char * mode);
    bool seek(size_t v, int origin = SEEK_SET);
    size_t tell();
    size_t read(void * data, size_t size);
    size_t write(const void * data, size_t size);
    void close();
    bool at_end();
    size_t get_size();

    bool is_open()
    {
        return (flags & CLOSED) == 0;
    }
};

class BufferedFile
{
public:
    BaseFile fp;

    size_t size;
    size_t pos;
    size_t buf_pos;
    size_t buf_size;
    void * buffer;

    BufferedFile();
    BufferedFile(const char * filename, const char * mode);
    ~BufferedFile();
    void buffer_data(size_t size);
    void open(const char * filename, const char * mode);
    bool seek(size_t v, int origin = SEEK_SET);
    size_t tell();
    bool is_open();
    size_t read(void * data, size_t size);
    size_t write(const void * data, size_t size);
    void close();
    bool at_end();
    size_t get_size();
};

#ifdef CHOWDREN_FILE_BUFFERING
typedef BufferedFile FSFile;
#else
typedef BaseFile FSFile;
#endif

bool read_file(const char * filename, char ** data, size_t * ret_size,
               bool binary = true);
bool read_file(const char * filename, chowstring & dst,
               bool binary = true);
bool read_file_c(const char * filename, char ** data, size_t * ret_size,
                 bool binary = true);

#endif // CHOWDREN_FILEIO_H
