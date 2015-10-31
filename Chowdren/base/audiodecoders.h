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
#include "stringcommon.h"
#include "fileio.h"
#include "platform.h"
#include "path.h"
#include <string.h>
#include "media.h"
#include "mathcommon.h"
#include "datastream.h"

namespace ChowdrenAudio
{
    inline long tell_func(void *fp)
    {
        FSFile * file = (FSFile*)fp;
        return file->tell();
    }

    inline size_t read_func(void * ptr, size_t size, size_t nmemb, void *fp)
    {
        FSFile * file = (FSFile*)fp;
        return file->read(ptr, size);
    }

    inline int seek_func(void *fp, size_t offset, int whence)
    {
        FSFile * file = (FSFile*)fp;
        if (file->seek(offset, whence))
            return 0;
        return 1;
    }
}

#define STB_VORBIS_NO_PUSHDATA_API
#define STB_VORBIS_NO_MEMORY
#define FILE void
#define ftell ChowdrenAudio::tell_func
#define fread ChowdrenAudio::read_func
#define fseek ChowdrenAudio::seek_func
#define fopen(name, mode) NULL
#define fclose(fp) {}
#define fgetc ChowdrenAudio::getc_func
#include "stb_vorbis.c"
#undef BUFFER_SIZE
#undef FILE
#undef ftell
#undef fseek
#undef fopen
#undef fclose

namespace ChowdrenAudio {

template <typename T>
void swap(T &val1, T &val2)
{
    val1 ^= val2;
    val2 ^= val1;
    val1 ^= val2;
}

class SoundDecoder
{
public:
    std::size_t samples;
    int channels;
    int sample_rate;

    virtual bool is_valid() = 0;
    virtual std::size_t read(signed short * data, std::size_t samples) = 0;
    virtual void seek(double value) = 0;
    virtual ~SoundDecoder() {};
    virtual void post_init() {};

    std::size_t get_samples()
    {
        return samples;
    }
};

class OggDecoder : public SoundDecoder
{
public:
    stb_vorbis * ogg;

    OggDecoder(FSFile & fp, size_t size)
    {
        int error;
        ogg = stb_vorbis_open_file_section((void*)&fp, 0, &error, NULL, size);
        if (ogg == NULL) {
            std::cout << "stb_vorbis_open_file_section failed: " << error
                << std::endl;
            return;
        }

        init();
    }

    OggDecoder(unsigned char * data, size_t size)
    {
        int error;
        ogg = stb_vorbis_open_memory(data, size, &error, NULL);
        if (ogg == NULL) {
            std::cout << "stb_vorbis_open_memory failed: " << error
                << std::endl;
            return;
        }

        init();
    }

    void init()
    {
        stb_vorbis_info info = stb_vorbis_get_info(ogg);
        channels = info.channels;
        sample_rate = info.sample_rate;
        samples = stb_vorbis_stream_length_in_samples(ogg) * channels;
        channels = clamp(channels, 1, 2);
    }

    ~OggDecoder()
    {
        if (ogg == NULL)
            return;
        stb_vorbis_close(ogg);
        ogg = NULL;
    }

    bool is_valid()
    {
        return ogg != NULL;
    }

    size_t read(signed short * sdata, std::size_t samples)
    {
        if (!(sdata && samples))
            return 0;
        unsigned int got = 0;
        int ret;
        while (samples > 0) {
            ret = stb_vorbis_get_samples_short_interleaved(ogg, channels,
                                                           sdata, samples);
            ret *= channels;
            if (ret <= 0)
                break;
            sdata += ret;
            samples -= ret;
            got += ret;
        }
        return got;
    }

    void seek(double value)
    {
        value = std::max(0.0, value);
        unsigned int sample_number = value * sample_rate;
        int ret = stb_vorbis_seek(ogg, sample_number);
        if (ret == 1)
            return;
        std::cout << "Seek failed: " << ret << " "
            << stb_vorbis_get_error(ogg) << " with time " << value
            << std::endl;
    }
};

template <class T>
class WavDecoderImpl : public SoundDecoder
{
private:
    T & file;
    int sample_size;
    int block_align;
    long data_start;
    long data_len;
    size_t rem_len;

public:
    WavDecoderImpl(T & fp)
    : file(fp), data_start(0)
    {
    }

    void init(size_t size)
    {
        unsigned char buffer[25];
        unsigned int length;
        if (!file.read(reinterpret_cast<char*>(buffer), 12) ||
            memcmp(buffer, "RIFF", 4) != 0 ||
            memcmp(buffer+8, "WAVE", 4) != 0)
        {
            std::cerr << "WAV: Invalid header" << std::endl;
            return;
        }

        while (!data_start) {
            char tag[4];
            if (!file.read(tag, 4))
                break;

            length = read_le32();

            if (memcmp(tag, "fmt ", 4) == 0 && length >= 16) {
                // data type (should be 1 for PCM data, 3 for float PCM data
                int type = read_le16();
                if(type != 0x0001 && type != 0x0003) {
                    std::cerr << "WAV: Invalid type" << std::endl;
                    break;
                }

                channels = read_le16();
                sample_rate = read_le32();
                file.seek(4, SEEK_CUR);
                block_align = read_le16();
                if(block_align == 0) {
                    std::cerr << "WAV: Invalid blockalign" << std::endl;
                    break;
                }
                sample_size = read_le16();
                if (sample_size != 16) {
                    std::cerr << "WAV: Invalid sample size" << std::endl;
                    break;
                }
                length -= 16;

            } else if (memcmp(tag, "data", 4) == 0) {
                data_start = file.tell();
                data_len = rem_len = length;
            }

            file.seek(length, SEEK_CUR);
        }

        if (data_start > 0) {
            samples = data_len / (sample_size / 8);
            file.seek(data_start);
        }
    }

    unsigned short read_le16()
    {
        unsigned char buffer[2];
        if (!file.read((char*)buffer, 2))
            return 0;
        return buffer[0] | (buffer[1]<<8);
    }

    unsigned int read_le32()
    {
        unsigned char buffer[4];
        if (!file.read((char*)buffer, 4))
            return 0;
        return buffer[0] | (buffer[1]<<8) | (buffer[2]<<16) | (buffer[3]<<24);
    }

	~WavDecoderImpl()
    {
    }

    bool is_valid()
    {
        return (data_start > 0);
    }

    size_t read(signed short * data, std::size_t samples)
    {
        unsigned int bytes = samples * (sample_size / 8);
        size_t rem;
        if (bytes <= rem_len)
            rem = bytes;
        else
            rem = rem_len;
        rem = (rem / block_align) * block_align;
        unsigned int got = file.read((char*)data, rem);
        got -= got%block_align;
        rem_len -= got;

#ifdef IS_BIG_ENDIAN
        unsigned char * datac = (unsigned char *)data;
        if (sample_size == 16) {
            for(std::streamsize i = 0; i < got; i+=2)
                swap(datac[i], datac[i+1]);
        } else if (sample_size == 32) {
            for(std::streamsize i = 0; i < got; i+=4) {
                swap(datac[i+0], datac[i+3]);
                swap(datac[i+1], datac[i+2]);
            }
        } else if (sample_size == 64) {
            for(std::streamsize i = 0; i < got; i+=8) {
                swap(datac[i+0], datac[i+7]);
                swap(datac[i+1], datac[i+6]);
                swap(datac[i+2], datac[i+5]);
                swap(datac[i+3], datac[i+4]);
            }
        }
#endif

        return got / (sample_size / 8);
    }

    void seek(double t)
    {
        long new_pos = t * sample_rate * (sample_size / 8) * channels;
        new_pos = std::max(0L, std::min(new_pos, data_len));
        file.seek(data_start + new_pos);
        rem_len = data_len - new_pos;
    }
};

class WavDecoderMemory : public WavDecoderImpl<ArrayStream>
{
public:
    ArrayStream stream;

    WavDecoderMemory(unsigned char * data, size_t size)
    : WavDecoderImpl(stream), stream((char*)data, size)
    {
        init(size);
    }
};

class WavDecoder : public WavDecoderImpl<FSFile>
{
public:
    WavDecoder(FSFile & fp, size_t size)
    : WavDecoderImpl(fp)
    {
        init(size);
    }
};

SoundDecoder * create_decoder(FSFile & fp, Media::AudioType type, size_t size)
{
    SoundDecoder * decoder;
    if (type == Media::WAV)
        decoder = new WavDecoder(fp, size);
    else if (type == Media::OGG)
        decoder = new OggDecoder(fp, size);
    else
        return NULL;
    if (decoder->is_valid())
        return decoder;
    std::cout << "Could not load sound" << std::endl;
    return NULL;
}

SoundDecoder * create_decoder(unsigned char * data, Media::AudioType type,
                              size_t size)
{
    SoundDecoder * decoder;
    if (type == Media::WAV)
        decoder = new WavDecoderMemory(data, size);
    else if (type == Media::OGG)
        decoder = new OggDecoder(data, size);
    else
        return NULL;
    if (decoder->is_valid())
        return decoder;
    std::cout << "Could not load sound" << std::endl;
    return NULL;
}

} // namespace ChowdrenAudio
