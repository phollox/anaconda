#include <string>
#include "stringcommon.h"
#include "fileio.h"
#include "platform.h"
#include "path.h"
#include <string.h>
#include "media.h"
#include "mathcommon.h"

#define USE_STB_VORBIS

namespace ChowdrenAudio
{
    static size_t read_func(void * ptr, size_t size, size_t nmemb, void *fp);
    static int seek_func(void *fp, size_t offset, int whence);
    static long tell_func(void *fp);
    static int getc_func(void * fp);
}

#ifdef USE_STB_VORBIS
#define STB_VORBIS_NO_PUSHDATA_API
#define STB_VORBIS_NO_MEMORY
#define FILE void
#define ftell ChowdrenAudio::tell_func
#define fread ChowdrenAudio::read_func
#define fseek ChowdrenAudio::seek_func
#define fopen(name, mode) {}
#define fclose(fp) {}
#define fgetc ChowdrenAudio::getc_func
#include "stb_vorbis.c"
#undef FILE
#undef ftell
#undef fseek
#undef fopen
#undef fclose
#else
#include <vorbis/vorbisfile.h>
#endif

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

#ifndef USE_STB_VORBIS
ov_callbacks callbacks = {
    read_func,
    seek_func,
    NULL,
    tell_func
};
#endif

class OggDecoder : public SoundDecoder
{
public:
    FSFile & fp;
    size_t start;
    size_t pos;
    size_t size;
#ifdef USE_STB_VORBIS
    stb_vorbis * ogg;
#else
    OggVorbis_File ogg_file;
    vorbis_info * ogg_info;
    int ogg_bitstream;
#endif

    OggDecoder(FSFile & fp, size_t size)
    : size(size), fp(fp)
    {
        start = fp.tell();
        pos = 0;

#ifdef USE_STB_VORBIS
        int error;
        ogg = stb_vorbis_open_file_section((void*)this, 0, &error, NULL, size);
        if (ogg == NULL) {
            std::cout << "stb_vorbis_open_file_section failed: " << error
                << std::endl;
            return;
        }

        stb_vorbis_info info = stb_vorbis_get_info(ogg);
        channels = info.channels;
        sample_rate = info.sample_rate;
        samples = stb_vorbis_stream_length_in_samples(ogg) * channels;
#else
        ogg_info = NULL;
        ogg_bitstream = 0;
 
        if (ov_open_callbacks((void*)this, &ogg_file, NULL, 0, callbacks) != 0)
        {
#ifndef NDEBUG
            std::cout << "ov_open_callbacks failed" << std::endl;
#endif
            return;
        }

        ogg_info = ov_info(&ogg_file, -1);
        if (!ogg_info) {
#ifndef NDEBUG
            std::cout << "ov_info failed" << std::endl;
#endif
            ov_clear(&ogg_file);
            return;
        }

        channels = ogg_info->channels;
        sample_rate = ogg_info->rate;
        samples = ov_pcm_total(&ogg_file, -1) * channels;
#endif
        channels = clamp(channels, 1, 2);
    }

    ~OggDecoder()
    {
#ifdef USE_STB_VORBIS
        if (ogg == NULL)
            return;
        stb_vorbis_close(ogg);
        ogg = NULL;
#else
        if (ogg_info)
            ov_clear(&ogg_file);
        ogg_info = NULL;
#endif
    }

    bool is_valid()
    {
#ifdef USE_STB_VORBIS
        return ogg != NULL;
#else
        return ogg_info != NULL;
#endif
    }

    size_t read(signed short * sdata, std::size_t samples)
    {
        if (!(sdata && samples))
            return 0;
#ifdef USE_STB_VORBIS
        unsigned int got = 0;
        int ret;
        while (samples > 0) {
            ret = stb_vorbis_get_samples_short_interleaved(ogg, channels,
                                                           sdata, samples) * 2;
            if (ret <= 0)
                break;
            sdata += ret;
            samples -= ret;
            got += ret;
        }
        return got;
#else
        unsigned int got = 0;
        int bytes = samples * 2;
        char * data = (char*)sdata;
        while(bytes > 0) {
#ifdef IS_BIG_ENDIAN
            int big_endian = 1;
#else
            int big_endian = 0;
#endif
            int res = ov_read(&ogg_file, &data[got], bytes,
                              big_endian, 2, 1, &ogg_bitstream);
            if(res <= 0)
                break;
            bytes -= res;
            got += res;
        }
        // XXX support exotic channel formats?
        return got / 2;
#endif
    }

    void seek(double value)
    {
        value = std::max(0.0, value);
#ifdef USE_STB_VORBIS
        unsigned int sample_number = value * sample_rate;
        int ret = stb_vorbis_seek(ogg, sample_number);
        if (ret == 1)
            return;
#else
        int ret = ov_time_seek(&ogg_file, value);
        if (ret == 0)
            return;
#endif
        std::cout << "Seek failed: " << ret << " with time " << value
            << std::endl;
    }
};

size_t read_func(void * ptr, size_t size, size_t nmemb, void *fp)
{
    OggDecoder * file = (OggDecoder*)fp;
    size *= nmemb;
#ifdef USE_STB_VORBIS
    return file->fp.read(ptr, size) / size;
#else
    size = std::min(file->size - file->pos, size);
    file->pos += size;
    return file->fp.read(ptr, size);
#endif
}

int getc_func(void * fp)
{
    OggDecoder * file = (OggDecoder*)fp;
    unsigned char c;
    if (file->fp.read(&c, 1) == 1)
        return c;
    return EOF;
}

int seek_func(void *fp, size_t offset, int whence)
{
    OggDecoder * file = (OggDecoder*)fp;
#ifdef USE_STB_VORBIS
    if (file->fp.seek(offset, whence))
        return 0;
    return 1;
#else
    switch (whence) {
        case SEEK_SET:
            break;
        case SEEK_END:
            offset = file->size - offset;
            break;
        case SEEK_CUR:
            offset += file->pos;
            break;
    }
    offset = std::min<size_t>(file->size, offset);
    offset = std::max<size_t>(0, offset);
    file->pos = offset;
    file->fp.seek(offset + file->start, SEEK_SET);
    return 1;
#endif
}

long tell_func(void *fp)
{
    OggDecoder * file = (OggDecoder*)fp;
#ifdef USE_STB_VORBIS
    return file->fp.tell();
#else
    return file->pos;
#endif
}

inline unsigned int read_le32(FSFile & file)
{
    unsigned char buffer[4];
    if (!file.read((char*)buffer, 4))
        return 0;
    return buffer[0] | (buffer[1]<<8) | (buffer[2]<<16) | (buffer[3]<<24);
}

inline unsigned short read_le16(FSFile & file)
{
    unsigned char buffer[2];
    if (!file.read((char*)buffer, 2))
        return 0;
    return buffer[0] | (buffer[1]<<8);
}

class WavDecoder : public SoundDecoder
{
private:
    FSFile & file;
    int sample_size;
    int block_align;
    long data_start;
    long data_len;
    size_t rem_len;

public:
    WavDecoder(FSFile & fp, size_t size)
    : file(fp), data_start(0)
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

            length = read_le32(file);

            if(memcmp(tag, "fmt ", 4) == 0 && length >= 16) {
                // data type (should be 1 for PCM data, 3 for float PCM data
                int type = read_le16(file);
                if(type != 0x0001 && type != 0x0003) {
                    std::cerr << "WAV: Invalid type" << std::endl;
                    break;
                }

                channels = read_le16(file);
                sample_rate = read_le32(file);
                file.seek(4, SEEK_CUR);
                block_align = read_le16(file);
                if(block_align == 0) {
                    std::cerr << "WAV: Invalid blockalign" << std::endl;
                    break;
                }
                sample_size = read_le16(file);
                if (sample_size != 16) {
                    std::cerr << "WAV: Invalid sample size" << std::endl;
                    break;
                }
                length -= 16;

            }
            else if(memcmp(tag, "data", 4) == 0) {
                data_start = file.tell();
                data_len = rem_len = length;
            }

            file.seek(length, SEEK_CUR);
        }

        if(data_start > 0) {
            samples = data_len / (sample_size / 8);
            file.seek(data_start);
        }
    }

    ~WavDecoder()
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
        if (rem_len >= bytes)
            rem = bytes;
        else
            rem = rem_len;
        rem /= block_align;
        size_t got = file.read((char*)data, rem*block_align);
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
        if (file.seek(data_start + new_pos))
            rem_len = data_len - new_pos;
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

} // namespace ChowdrenAudio
