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

#ifndef CHOWDREN_DATASTREAM_H
#define CHOWDREN_DATASTREAM_H

#include <sstream>
#include "platform.h"
#include <string.h>
#include <algorithm>

template <class T>
class BaseStream
{
public:
    BaseStream()
    {
    }

    // read

    void read_string(std::string & str, size_t len)
    {
        str.resize(len, 0);
        read(&str[0], len);
    }

    void read(std::stringstream & out, size_t len)
    {
        std::string data;
        read_string(data, len);
        out << data;
    }

    char read_int8()
    {
        char v;
        if (!read(&v, 1))
            return 0;
        return v;
    }

    unsigned char read_uint8()
    {
        return (unsigned char)read_int8();
    }

    short read_int16()
    {
        unsigned char data[2];
        if (!read((char*)data, 2))
            return 0;
        return data[0] | (data[1] << 8);
    }

    unsigned short read_uint16()
    {
        return (unsigned short)read_int16();
    }

    int read_int32()
    {
        unsigned char data[4];
        if (!read((char*)data, 4))
            return 0;
        return data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
    }

    unsigned int read_uint32()
    {
        return (unsigned int)read_int32();
    }

    float read_float()
    {
        float f;
        int i = read_int32();
        memcpy(&f, &i, sizeof(float));
        return f;
    }

    void read_c_string(std::string & str)
    {
        read_delim(str, '\0');
    }

    // write

    void write_int8(char v)
    {
        write(&v, 1);
    }

    void write_uint8(unsigned char v)
    {
        write_int8(char(v));
    }

    void write_int16(short v)
    {
        unsigned char data[2];
        data[0] = v & 0xFF;
        data[1] = (v >> 8) & 0xFF;
        write((char*)&data[0], 2);
    }

    void write_uint16(unsigned short v)
    {
        write_int16(short(v));
    }

    void write_int32(int v)
    {
        unsigned char data[4];
        data[0] = v & 0xFF;
        data[1] = (v >> 8) & 0xFF;
        data[2] = (v >> 16) & 0xFF;
        data[3] = (v >> 24) & 0xFF;
        write((char*)&data[0], 4);
    }

    void write_uint32(unsigned int v)
    {
        write_int32(int(v));
    }


    void write_string(const std::string & str)
    {
        write(&str[0], str.size());
    }

    void read_delim(std::string & line, char delim)
    {
        while (true) {
            char c;
            if (!read(&c, 1))
                break;
            if (c == delim)
                break;
            line += c;
        }
    }

    void read_line(std::string & str)
    {
        read_delim(str, '\n');
    }

    // subclasses implement these
    void write(const char * data, size_t len)
    {
        ((T*)this)->write(data, len);
    }

    unsigned int read(char * data, size_t len)
    {
        return ((T*)this)->read(data, len);
    }

    bool seek(size_t pos)
    {
        return ((T*)this)->seek(pos);
    }

    bool seek(size_t pos, int whence)
    {
        return ((T*)this)->seek(pos, whence);
    }

    bool at_end()
    {
        return ((T*)this)->at_end();
    }

    size_t tell()
    {
        return ((T*)this)->tell();
    }
};

class FileStream : public BaseStream<FileStream>
{
public:
    FSFile & fp;

    FileStream(FSFile & fp)
    : fp(fp)
    {
    }

    unsigned int read(char * data, size_t len)
    {
        return fp.read(data, len);
    }

    void seek(size_t pos)
    {
        fp.seek(pos);
    }

    void seek(size_t pos, int whence)
    {
        fp.seek(pos, whence);
    }

    bool at_end()
    {
        return fp.at_end();
    }

    void write(const char * data, size_t len)
    {
        fp.write(data, len);
    }

    size_t tell()
    {
        return fp.tell();
    }
};

class DataStream : public BaseStream<DataStream>
{
public:
    std::stringstream & stream;

    DataStream(std::stringstream & stream)
    : stream(stream)
    {
    }

    unsigned int read(char * data, size_t len)
    {
        stream.read(data, len);
        return stream.gcount();
    }

    void seek(size_t pos)
    {
        stream.seekg(pos);
    }

    void seek(size_t pos, int whence)
    {
        switch (whence) {
            case SEEK_SET:
                stream.seekg(pos, std::stringstream::beg);
                break;
            case SEEK_CUR:
                stream.seekg(pos, std::stringstream::cur);
                break;
            case SEEK_END:
                stream.seekg(pos, std::stringstream::end);
                break;
        }
    }

    bool at_end()
    {
        return stream.peek() == EOF;
    }

    void write(const char * data, size_t len)
    {
        stream.write(data, len);
    }

    size_t tell()
    {
        return stream.tellg();
    }
};

class WriteStream : public DataStream
{
public:
    std::stringstream stream;

    WriteStream()
    : DataStream(stream)
    {
    }

    ~WriteStream()
    {
    }

    std::string get_string()
    {
        return stream.str();
    }

    void save(FSFile & fp)
    {
        std::string data = stream.str();
        if (data.empty())
            return;
        fp.write(&data[0], data.size());
    }
};

class StringStream : public BaseStream<StringStream>
{
public:
    const std::string & str;
    size_t pos;

    StringStream(const std::string & str)
    : str(str), pos(0)
    {
    }

    unsigned int read(char * data, size_t len)
    {
        if (str.size() - pos < len)
            return 0;
        memcpy(data, &str[pos], len);
        pos += len;
        return len;
    }

    void seek(size_t p)
    {
        pos = std::max(size_t(0), std::min(p, str.size()));
    }

    bool at_end()
    {
        return pos == str.size();
    }

    void write(const char * data, size_t len)
    {
    }

    size_t tell()
    {
        return pos;
    }
};

class ArrayStream : public BaseStream<ArrayStream>
{
public:
    char * array;
    size_t size;
    size_t pos;

    ArrayStream(char * array, size_t size)
    : array(array), size(size), pos(0)
    {
    }

    ArrayStream()
    : pos(0), array(NULL), size(0)
    {
    }

    void init(char * array, size_t size)
    {
        this->array = array;
        this->size = size;
        pos = 0;
    }

    unsigned int read(char * data, size_t len)
    {
        if (size - pos < len)
            return 0;
        memcpy(data, &array[pos], len);
        pos += len;
        return len;
    }

    void seek(size_t p)
    {
        pos = std::max(size_t(0), std::min(p, size));
    }

    void seek(size_t p, int whence)
    {
        switch (whence) {
            case SEEK_SET:
                seek(p);
                break;
            case SEEK_CUR:
                seek(pos + p);
                break;
            case SEEK_END:
                seek(size - p);
                break;
        }
    }

    bool at_end()
    {
        return pos == size;
    }

    void write(const char * data, size_t len)
    {
    }

    size_t tell()
    {
        return pos;
    }
};

#endif // CHOWDREN_DATASTREAM_H
