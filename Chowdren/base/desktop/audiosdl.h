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
#include <algorithm>

#ifdef CHOWDREN_IS_EMSCRIPTEN
#include <emscripten/emscripten.h>
#else // CHOWDREN_IS_EMSCRIPTEN
#include <SDL_thread.h>
#include <SDL_mutex.h>
#include <SDL_messagebox.h>
#include <SDL_audio.h>
#include <SDL.h>
#endif // CHOWDREN_IS_EMSCRIPTEN

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <math.h>
#include "../types.h"
#include "../audiodecoders.h"

#define BUFFER_COUNT 3

#define USE_THREAD_PRELOAD
#define USE_FILE_PRELOAD

namespace ChowdrenAudio {

class SoundStream;
class Sound;

#define MAX_SOUNDS 64

class AudioDevice
{
public:

    SDL_Thread * streaming_thread;
    SDL_mutex * stream_mutex;
    volatile bool closing;
    SoundStream * streams[MAX_SOUNDS];
    Sound * sounds[MAX_SOUNDS];
    SDL_AudioDeviceID dev;
#ifdef USE_THREAD_PRELOAD
    SDL_cond * stream_cond;
    SDL_mutex * stream_cond_mutex;
#endif

    void open();
    static int _stream_update(void * data);
    void stream_update();
    void add_stream(SoundStream*);
    void remove_stream(SoundStream*);
    void close();
};

static AudioDevice global_device;


class Listener
{
public:
    static volatile float volume;
    static float mixer_volume;

    static void set_volume(float value)
    {
        volume = std::max(0.0f, std::min(1.0f, value));
    }

    static float get_volume()
    {
        return volume;
    }
};

volatile float Listener::volume = 1.0f;
float Listener::mixer_volume;

void open_audio()
{
    global_device.open();
}

void close_audio()
{
    global_device.close();
}

static signed short * scratch_buffer;
static unsigned int scratch_size;

static inline signed short * get_scratch(unsigned int size)
{
    if (scratch_size < size) {
        delete[] scratch_buffer;
        scratch_buffer = new signed short[size];
        scratch_size = size;
    }
    return scratch_buffer;
}

static inline void to_float(signed short * src, float * dst,
                            unsigned int samples)
{
    for (unsigned int i = 0; i < samples; ++i) {
        dst[i] = src[i] / float(0x8000); 
    }
}

class Sample
{
public:
    float * data;
    unsigned int samples;
    unsigned int sample_rate;
    unsigned int channels;

    Sample(FSFile & fp, Media::AudioType type, size_t size);
    Sample(unsigned char * data, Media::AudioType type, size_t size);
    ~Sample();
};

template <class T>
inline T clamp(T value)
{
    return std::min<T>(1, std::max<T>(0, value));
}

double get_pan_factor(double pan)
{
    if (pan == 1.0)
        return 1.0;
    else if (pan == 0.0)
        return 0.0;
    // directsound algorithmic scale, taken from wine sources
    return clamp(pow(2.0, (pan * 10000) / 600.0) / 65535.0);
}

static float * resample_scratch = NULL;
static unsigned int resample_size = 0;

static void upsample_float_mono(float * in_src, unsigned int samples,
                                double rate_incr)
{
    unsigned int len_cvt = samples * sizeof(float);
    const int srcsize = len_cvt - 64;
    const int dstsize = (int) (((double)(len_cvt/4)) * rate_incr) * 4;
    register int eps = 0;
    float *dst = ((float *) (in_src + dstsize)) - 1;
    const float *src = ((float *) (in_src + len_cvt)) - 1;
    const float *target = ((const float *) in_src);
    float sample0 = src[0];
    float last_sample0 = sample0;
    while (dst >= target) {
        dst[0] = sample0;
        dst--;
        eps += srcsize;
        if ((eps << 1) >= dstsize) {
            src--;
            sample0 = (float) ((((double) src[0]) +
                                ((double) last_sample0)) * 0.5);
            last_sample0 = sample0;
            eps -= dstsize;
        }
    }
}

static void downsample_float_mono(float * in_src, unsigned int samples,
                                  double rate_incr)
{
    unsigned int len_cvt = samples * sizeof(float);
    const int srcsize = len_cvt - 64;
    const int dstsize = (int) (((double)(len_cvt/4)) * rate_incr) * 4;
    register int eps = 0;
    float *dst = (float *) in_src;
    const float *src = (float *) in_src;
    const float *target = (const float *) (in_src + dstsize);
    float sample0 = src[0];
    float last_sample0 = sample0;
    while (dst < target) {
        src++;
        eps += dstsize;
        if ((eps << 1) >= srcsize) {
            dst[0] = sample0;
            dst++;
            sample0 = (float) ((((double) src[0]) +
                                ((double) last_sample0)) * 0.5);
            last_sample0 = sample0;
            eps -= srcsize;
        }
    }
}

static void upsample_float_stereo(float * in_src, unsigned int samples,
                                  double rate_incr)
{
    unsigned int len_cvt = samples * sizeof(float);
    const int srcsize = len_cvt - 128;
    const int dstsize = (int) (((double)(len_cvt/8)) * rate_incr) * 8;
    register int eps = 0;
    float *dst = ((float *) (in_src + dstsize)) - 2;
    const float *src = ((float *) (in_src + len_cvt)) - 2;
    const float *target = ((const float *) in_src);
    float sample1 = src[1];
    float sample0 = src[0];
    float last_sample1 = sample1;
    float last_sample0 = sample0;
    while (dst >= target) {
        dst[1] = sample1;
        dst[0] = sample0;
        dst -= 2;
        eps += srcsize;
        if ((eps << 1) >= dstsize) {
            src -= 2;
            sample1 = (float) ((((double) src[1]) +
                                ((double) last_sample1)) * 0.5);
            sample0 = (float) ((((double) src[0]) +
                                ((double) last_sample0)) * 0.5);
            last_sample1 = sample1;
            last_sample0 = sample0;
            eps -= dstsize;
        }
    }
}

static void downsample_float_stereo(float * in_src, unsigned int samples,
                                    double rate_incr)
{
    unsigned int len_cvt = samples * sizeof(float);
    const int srcsize = len_cvt - 128;
    const int dstsize = (int) (((double)(len_cvt/8)) * rate_incr) * 8;
    register int eps = 0;
    float *dst = (float *) in_src;
    const float *src = (float *) in_src;
    const float *target = (const float *) (in_src + dstsize);
    float sample0 = src[0];
    float sample1 = src[1];
    float last_sample0 = sample0;
    float last_sample1 = sample1;
    while (dst < target) {
        src += 2;
        eps += dstsize;
        if ((eps << 1) >= srcsize) {
            dst[0] = sample0;
            dst[1] = sample1;
            dst += 2;
            sample0 = (float) ((((double) src[0]) +
                                ((double) last_sample0)) * 0.5);
            sample1 = (float) ((((double) src[1]) +
                                ((double) last_sample1)) * 0.5);
            last_sample0 = sample0;
            last_sample1 = sample1;
            eps -= srcsize;
        }
    }
}

static inline float * get_resample_scratch(unsigned int size)
{
    if (resample_size < size) {
        resample_size = size;
        delete[] resample_scratch;
        resample_scratch = new float[resample_size];
    }
    return resample_scratch;
}

inline unsigned int get_resampled_size(unsigned int samples,
                                       unsigned int channels,
                                       double rate_incr)
{
    return (int) (((double)(samples/(channels))) * rate_incr) * (4*channels);
}

#define FIXPOINT_FRAC_BITS 12
#define FIXPOINT_FRAC_MUL (1 << FIXPOINT_FRAC_BITS)
#define FIXPOINT_FRAC_MASK ((1 << FIXPOINT_FRAC_BITS) - 1)

inline float point_mono(const float * vals, unsigned int frac)
{
    return vals[0];
}

inline float lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

inline float lerp_mono(const float * vals, unsigned int frac)
{
    return lerp(vals[0], vals[1], frac * (1.0f / FIXPOINT_FRAC_MUL));
}

inline float point_stereo(const float * vals, unsigned int frac)
{
    return vals[0];
}

inline float lerp_stereo(const float * vals, unsigned int frac)
{
    return lerp(vals[0], vals[2], frac * (1.0f / FIXPOINT_FRAC_MUL));
}

#ifdef _MSC_VER
#define restrict __restrict
#else
#define restrict __restrict__
#endif

typedef float (*sampler_op)(const float*, unsigned int);

template <sampler_op f>
void resample_generic_mono(const float * src, unsigned int frac,
                           unsigned int increment, float * restrict dst,
                           unsigned int dst_samples)
{
    for (unsigned int i = 0; i < dst_samples; i++) {
        dst[i] = f(src, frac);
        frac += increment;
        src += frac >> FIXPOINT_FRAC_BITS;
        frac &= FIXPOINT_FRAC_MASK;
    }
}

template <sampler_op f>
void resample_generic_stereo(const float * src, unsigned int frac,
                             unsigned int increment, float * restrict dst,
                             unsigned int dst_samples)
{
    for (unsigned int i = 0; i < dst_samples; i += 2) {
        dst[i] = f(src, frac);
        dst[i+1] = f(src+1, frac);
        frac += increment;
        src += (frac >> FIXPOINT_FRAC_BITS) * 2;
        frac &= FIXPOINT_FRAC_MASK;
    }
}

#define resample_point_mono resample_generic_mono<point_mono>
#define resample_point_stereo resample_generic_stereo<point_stereo>
#define resample_linear_mono resample_generic_mono<lerp_mono>
#define resample_linear_stereo resample_generic_stereo<lerp_stereo>

class SoundBase
{
public:
    enum Status
    {
        Stopped,
        Paused,
        Playing
    };

    enum SoundFlags
    {
        LOOP = 1 << 0,
        STREAM_LOOP = 1 << 1,
        PLAY = 1 << 2,
        PAUSE = 1 << 3,
        DESTROY = 1 << 4,
        DESTROY_STREAM = 1 << 5,
        USE_CVT = 1 << 6
    };

    float left_gain, right_gain;
    float volume;
    float pitch;
    int channels;
    bool closed;

    double rate_incr;
    void (*cvt_func)(float *, unsigned int, double);
    unsigned int step;
    unsigned int frac;

    unsigned int sample_rate;
    volatile unsigned int flags;
    float * data;
    unsigned int data_offset;
    unsigned int data_size;
    int data_end;
    unsigned int start_ticks;
    unsigned int pause_ticks;

    SoundBase()
    : left_gain(1.0f), right_gain(1.0f), flags(0), data_offset(0),
      data_size(0), data_end(-1), closed(false), frac(0),
      pitch(1.0f), volume(1.0f)
    {
    }

    void set_pitch(float pitch)
    {
        if (pitch == this->pitch)
            return;
        this->pitch = pitch;
        update_rate();
    }

    float get_pitch()
    {
        return pitch;
    }

    void set_volume(float value)
    {
        volume = value;
    }

    float get_volume()
    {
        return volume;
    }

    void set_pan(double value)
    {
        if (value > 1.0f)
            value = 1.0f;
        else if (value < -1.0f)
            value = -1.0f;
        // XXX we probably need to put locks on this
        left_gain = get_pan_factor(clamp(1.0f - value));
        right_gain = get_pan_factor(clamp(1.0f + value));
    }

    void update_rate()
    {
        if (sample_rate == 44100) {
            flags &= ~USE_CVT;
            return;
        }

        flags |= USE_CVT;
        float step = pitch * (sample_rate / 44100.0f);
        
        unsigned int step_fixed;
        if (step > 255.0f)
            step_fixed = 255 << FIXPOINT_FRAC_BITS;
        else
            step_fixed = (int)(step * FIXPOINT_FRAC_MUL);
        if (step_fixed == 0)
            step_fixed = 1;
        this->step = step_fixed;
    }

    float * resample(unsigned int dst_samples)
    {
        float * src = &data[data_offset];
		if (resample_size < dst_samples) {
			resample_size = dst_samples;
            delete[] resample_scratch;
            resample_scratch = new float[resample_size];
        }
        if (channels == 2) {
            resample_point_stereo(src, frac, step, resample_scratch,
                                  dst_samples);
        } else {
            resample_point_mono(src, frac, step, resample_scratch,
                                dst_samples);
        }
		return resample_scratch;
    }

    void update_sound_cvt(float * buf, unsigned int size)
    {
        float mix_vol = Listener::mixer_volume * volume;
        float pan[2] = {left_gain * mix_vol, right_gain * mix_vol};

        while (size > 0) {
            unsigned int end;
            bool has_end = data_end != -1 &&
                           (unsigned int)data_end >= data_offset;
            if (has_end)
                end = (unsigned int)data_end;
            else
                end = data_size;
            unsigned int samples = (end - data_offset) / channels;

            // number of samples that will be needed from source buffer
            unsigned int src_size = samples;
            src_size *= step;
            src_size += frac + FIXPOINT_FRAC_MASK;
            src_size >>= FIXPOINT_FRAC_BITS;
            // src_size += buffer_pad + buffer_pre_pad;
            src_size = std::min(src_size, samples);

            // number of samples put in destination buffer
            unsigned int dst_size = src_size;
            // dst_size -= buffer_pad + buffer_pre_pad;
            dst_size <<= FIXPOINT_FRAC_BITS;
            dst_size -= frac;
            dst_size = ((dst_size + (step - 1)) / step);
            dst_size = std::min(dst_size, size / 2);
            float * new_samples = resample(dst_size * channels);

            if (channels == 2) {
                for (unsigned int i = 0; i < dst_size; i++) {
                    *(buf++) += (*(new_samples++)) * pan[0];
                    *(buf++) += (*(new_samples++)) * pan[1];
                }
            } else {
                for (unsigned int i = 0; i < dst_size; i++) {
                    float v = (*(new_samples++));
                    *(buf++) += v * pan[0];
                    *(buf++) += v * pan[1];
                }
            }

            size -= dst_size * 2;

            frac += step * dst_size;
            unsigned read_samples = (frac >> FIXPOINT_FRAC_BITS) * channels;
            data_offset += read_samples;
            frac &= FIXPOINT_FRAC_MASK;

            if (data_offset < end)
                continue;
            if (flags & LOOP && !has_end) {
                data_offset %= end;
            } else {
                flags &= ~PLAY;
                break;
            }
        }
    }

    void update_sound(float * buf, unsigned int size)
    {
        if (!(flags & PLAY))
            return;
        if (flags & PAUSE)
            return;
        if (flags & USE_CVT) {
            update_sound_cvt(buf, size);
            return;
        }
        float mix_vol = Listener::mixer_volume * volume;
        float pan[2] = {left_gain * mix_vol, right_gain * mix_vol};
        while (size > 0) {
            unsigned int end;
            bool has_end = data_end != -1 &&
                           (unsigned int)data_end >= data_offset;
            if (has_end)
                end = (unsigned int)data_end;
            else
                end = data_size;
            unsigned int new_offset, samples;
            float * src = &data[data_offset];
            if (channels == 2) {
                new_offset = std::min(data_offset + size, end);
                samples = new_offset - data_offset;
                for (unsigned int i = 0; i < samples / 2; i++) {
                    *(buf++) += (*(src++)) * pan[0];
                    *(buf++) += (*(src++)) * pan[1];
                }

                size -= samples;
            } else {
                new_offset = std::min(data_offset + size / 2, end);
                samples = new_offset - data_offset;
                for (unsigned int i = 0; i < samples; i++) {
                    float val = *(src++);
                    *(buf++) += val * pan[0];
                    *(buf++) += val * pan[1];
                }

                size -= samples * 2;
            }

            data_offset = new_offset;
            if (data_offset != end)
                continue;
            if (flags & LOOP && !has_end) {
                data_offset = 0;
            } else {
                flags &= ~PLAY;
                break;
            }
        }
    }

    virtual Status get_status()
    {
        if (flags & PLAY)
            return Playing;
        if (flags & PAUSE)
            return Paused;
        return Stopped;
    }

    void set_frequency(int value)
    {
        set_pitch(double(value) / get_sample_rate());
    }

    int get_frequency()
    {
        return get_pitch() * get_sample_rate();
    }

    virtual void play()
    {
        if (flags & PAUSE) {
            start_ticks = SDL_GetTicks() - pause_ticks;
            flags &= ~PAUSE;
            return;
        }
        // XXX race condition
        flags |= PLAY;
        start_ticks = SDL_GetTicks();
    }

    unsigned int get_ticks()
    {
        if (flags & PAUSE)
            return pause_ticks;
        return SDL_GetTicks() - start_ticks;
    }

    virtual void pause()
    {
        if (flags & PAUSE)
            return;
        // XXX race condition
        flags |= PAUSE;
        pause_ticks = SDL_GetTicks() - start_ticks;
    }

    virtual void stop()
    {
        // XXX race condition
        closed = true;
        flags &= ~PLAY;
    }

    virtual bool get_loop()
    {
        return flags & LOOP;
    }

    virtual void set_loop(bool loop)
    {
        // XXX race condition
        if (loop)
            flags |= LOOP;
        else
            flags &= ~LOOP;
    }

    virtual int get_sample_rate() = 0;
    virtual void set_playing_offset(double) = 0;
    virtual double get_playing_offset() = 0;
    virtual double get_duration() = 0;

    virtual ~SoundBase()
    {
    }

    void destroy()
    {
        stop();
        flags |= DESTROY;
    }
};

class Sound : public SoundBase
{
public:
    Sample & sample;

    Sound(Sample & sample) : sample(sample), SoundBase()
    {
        channels = sample.channels;
        sample_rate = sample.sample_rate;
        data = sample.data;
        data_size = sample.samples;

        update_rate();

        for (int i = 0; i < MAX_SOUNDS; ++i) {
            if (global_device.sounds[i] != NULL)
                continue;
            global_device.sounds[i] = this;
            break;
        }
    }

    ~Sound()
    {
    }

    double get_playing_offset()
    {
        unsigned int ticks = get_ticks();
        unsigned int max_ticks = (sample.samples * 1000)
                                 / sample_rate / channels;
        if (flags & LOOP)
            ticks %= max_ticks;
        else
            ticks = std::min(ticks, max_ticks);
        return ticks / 1000.0;
    }

    void set_playing_offset(double offset)
    {
        // XXX race condition
        data_offset = int(offset * sample.sample_rate) * channels;
        unsigned int ticks = int(offset * 1000.0);
        start_ticks = SDL_GetTicks() - ticks;
        pause_ticks = ticks;
    }

    double get_duration()
    {
        return double(sample.samples) / sample.sample_rate / sample.channels;
    }

    int get_sample_rate()
    {
        return sample.sample_rate;
    }
};

#define LOCK_STREAM SDL_LockMutex(global_device.stream_mutex)
#define UNLOCK_STREAM SDL_UnlockMutex(global_device.stream_mutex)

#define BUFFER_SIZE ((sample_rate / BUFFER_COUNT) * channels)

struct AudioPreload
{
    unsigned int sample_rate;
    unsigned int channels;
    unsigned int samples;
    size_t size;
};

static hash_map<std::string, AudioPreload> audio_preloads;

void create_audio_preload(const std::string & in_path)
{
    std::string path = convert_path(in_path);
    std::cout << "Preload: " << path << std::endl;
    FSFile fp(path.c_str(), "r");
    size_t size = platform_get_file_size(path.c_str());
    AudioPreload & preload = audio_preloads[path];
    preload.size = size;
    if (size <= 0)
        return;
    SoundDecoder * decoder = create_decoder(fp, get_audio_type(path), size);
    preload.sample_rate = decoder->sample_rate;
    preload.samples = decoder->get_samples();
    preload.channels = decoder->channels;
    delete decoder;
}

AudioPreload * get_audio_preload(const std::string & path)
{
    hash_map<std::string, AudioPreload>::iterator it = audio_preloads.find(path);
    if (it == audio_preloads.end()) {
        std::cout << "No preloads for " << path << std::endl;
        return NULL;
    }
    return &(it->second);
}

class SoundStream : public SoundBase
{
public:
    AssetFile fp;
    SoundDecoder * file;
    bool playing;
    double seek_time;
    unsigned int current_buf;

    unsigned int samples;

    volatile bool fill_now;
    volatile bool with_seek;

#ifdef USE_FILE_PRELOAD
    volatile Media::AudioType decoder_type;
    volatile size_t decoder_size;
    std::string decoder_filename;
#endif 

    SoundStream(size_t offset, Media::AudioType type, size_t size)
    : SoundBase()
    {
        fp.open();
        fp.seek(offset);
        init(create_decoder(fp, type, size));
    }

#ifdef USE_FILE_PRELOAD
    SoundStream(const std::string & path, Media::AudioType type, size_t size)
    : SoundBase()
    {
        AudioPreload * preload = get_audio_preload(path);
        if (preload == NULL) {
            fp.open(path.c_str(), "r");
            init(create_decoder(fp, type, size));
            return;
        }
        decoder_type = type;
        decoder_size = size;
        decoder_filename = path;
        file = NULL;
        playing = fill_now = with_seek = false;
        flags |= LOOP;

        sample_rate = preload->sample_rate;
        channels = preload->channels;
        samples = preload->samples;

        update_rate();

        data_size = (sample_rate / BUFFER_COUNT) * BUFFER_COUNT * channels;
        data = new float[data_size];

        // LOCK_STREAM;
        for (int i = 0; i < MAX_SOUNDS; ++i) {
            if (global_device.streams[i] != NULL)
                continue;
            global_device.streams[i] = this;
            break;
        }
        // UNLOCK_STREAM;
    }
#else
    SoundStream(const std::string & path, Media::AudioType type, size_t size)
    : SoundBase()
    {
        fp.open(path.c_str(), "r");
        init(create_decoder(fp, type, size));
    }
#endif

    void init(SoundDecoder * decoder)
    {
        file = decoder;
        playing = fill_now = with_seek = false;
        flags |= LOOP;

		sample_rate = decoder->sample_rate;
		channels = decoder->channels;
        samples = decoder->get_samples();

        update_rate();

        data_size = (sample_rate / BUFFER_COUNT) * BUFFER_COUNT * channels;
        data = new float[data_size];

        // LOCK_STREAM;
        for (int i = 0; i < MAX_SOUNDS; ++i) {
            if (global_device.streams[i] != NULL)
                continue;
            global_device.streams[i] = this;
            break;
        }
        // UNLOCK_STREAM;
    }

    bool get_loop()
    {
        return (flags & STREAM_LOOP) != 0;
    }

    void set_loop(bool loop)
    {
        // XXX race condition
        if (loop)
            flags |= STREAM_LOOP;
        else
            flags &= ~STREAM_LOOP;
    }

    ~SoundStream()
    {
        stop();
        delete[] data;
        delete file;
    }

    void play()
    {
        // XXX race condition
        if (flags & PAUSE) {
            flags &= ~PAUSE;
            return;
        }

        fill_now = true;
        playing = true;
        SDL_CondBroadcast(global_device.stream_cond);
    }

    void stop()
    {
        if (!playing)
            return;
        closed = true;
        // LOCK_STREAM;
        playing = false;
        SoundBase::stop();
        // UNLOCK_STREAM;
    }

    Status get_status()
    {
        Status status = SoundBase::get_status();
        if (status == Stopped && fill_now)
            return Playing;
        return status;
    }

    void set_playing_offset(double t)
    {
        unsigned int ticks = int(t * 1000.0);
        start_ticks = SDL_GetTicks() - ticks;
        pause_ticks = ticks;
        // LOCK_STREAM;
        seek_time = t;
        fill_now = true;
        with_seek = true;
        SDL_CondBroadcast(global_device.stream_cond);
        // UNLOCK_STREAM;
    }

    double get_playing_offset()
    {
        if (get_status() == Stopped)
            return get_duration();
        unsigned int ticks = get_ticks();
        unsigned int max_ticks = (samples * 1000) / sample_rate / channels;
        if (flags & STREAM_LOOP)
            ticks %= max_ticks;
        else
            ticks = std::min(ticks, max_ticks);
        return ticks / 1000.0;
    }

    double get_duration()
    {
        return double(samples) / sample_rate / channels;
    }

    int get_sample_rate()
    {
        return sample_rate;
    }

    void update()
    {
        if (!playing)
            return;

        if (fill_now) {
            if (file == NULL) {
                fp.open(decoder_filename.c_str(), "r");
                file = create_decoder(fp, decoder_type, decoder_size);
            }
            if (with_seek) {
                file->seek(seek_time);
            }
            fill_queue();
            SoundBase::play();
            fill_now = with_seek = false;
            return;
        }

        if (!(flags & PLAY))
            return;
        if (flags & PAUSE)
            return;

        int next = data_offset / BUFFER_SIZE;
        if (next == current_buf)
            return;

        while (current_buf != next) {
            if (data_end == -1)
                fill_buffer(current_buf);
            current_buf = (current_buf+1) % BUFFER_COUNT;
        }
    }

    bool fill_buffer(unsigned int buffer_num)
    {
        unsigned int samples = BUFFER_SIZE;
        unsigned int byte_offset = buffer_num * samples;
        float * offset = &data[byte_offset];
        signed short * scratch = get_scratch(samples);
        while (samples > 0) {
            unsigned int read = file->read(scratch, samples);
            to_float(scratch, offset, read);
            if (read == samples)
                break;
            if (flags & STREAM_LOOP) {
                file->seek(0);
                samples -= read;
                offset += read;
            } else {
                data_end = byte_offset + read;
                break;
            }
        }

        return data_end != -1;
    }

    void fill_queue()
    {
        current_buf = 0;
        data_end = -1;
        for (int i = 0; i < BUFFER_COUNT; ++i) {
            if (fill_buffer(i)) {
                break;
            }
        }
    }
};

// audio device implementation

static void audio_callback(void * user, Uint8 * in_stream, int len)
{
    memset(in_stream, 0, len);
    float * stream = (float*)in_stream;
    len /= 4;
    SDL_LockAudioDevice(global_device.dev);
    Listener::mixer_volume = Listener::volume;
    for (int i = 0; i < MAX_SOUNDS; ++i) {
        Sound * sound = global_device.sounds[i];
        if (sound == NULL)
            continue;
        if (sound->flags & SoundBase::DESTROY) {
            delete sound;
            global_device.sounds[i] = NULL;
            continue;
        }
        sound->update_sound(stream, len);
    }
    for (int i = 0; i < MAX_SOUNDS; ++i) {
        SoundStream * sound = global_device.streams[i];
        if (sound == NULL)
            continue;
        if (sound->flags & SoundBase::DESTROY_STREAM)
            continue;
        if (sound->flags & SoundBase::DESTROY) {
            sound->flags |= SoundBase::DESTROY_STREAM;
            continue;
        }
        sound->update_sound(stream, len);
    }

    // hard limiter, like what dsound does.
    for (int i = 0; i < len; ++i) {
        float val = stream[i];
        if (val > 1)
            val = 1;
        else if (val < -1)
            val = -1;
        stream[i] = val;
    }

    SDL_UnlockAudioDevice(global_device.dev);
}

void AudioDevice::open()
{
    closing = false;
    streaming_thread = NULL;

    SDL_AudioSpec want, have;

    SDL_zero(want);
    want.freq = 44100;
    want.format = AUDIO_F32SYS;
    want.channels = 2;
    want.samples = 4096;
    want.callback = audio_callback;

    SDL_InitSubSystem(SDL_INIT_AUDIO);

    dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if (dev == 0) {
        std::cout << "Device open failed: " << SDL_GetError() << std::endl;
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Audio error",
            "Could not open audio device. Ensure that at least 1 audio "
            "device is enabled.", NULL);
        exit(EXIT_FAILURE);
        return;
    }

    SDL_PauseAudioDevice(dev, 0);
    stream_mutex = SDL_CreateMutex();

#ifdef USE_THREAD_PRELOAD
    stream_cond = SDL_CreateCond();
    stream_cond_mutex = SDL_CreateMutex();
#endif
    streaming_thread = SDL_CreateThread(_stream_update, "Stream thread",
                                        (void*)this);
}

void pause_audio()
{
    if (global_device.dev == 0)
        return;
    SDL_PauseAudioDevice(global_device.dev, 1);
}

void resume_audio()
{
    if (global_device.dev == 0)
        return;
    SDL_PauseAudioDevice(global_device.dev, 0);
}

void AudioDevice::close()
{
    closing = true;
    if (streaming_thread != NULL) {
        int ret;
        SDL_WaitThread(streaming_thread, &ret);
    }

    SDL_DestroyMutex(stream_mutex);
    SDL_CloseAudioDevice(dev);
}

void AudioDevice::stream_update()
{
    while (!closing) {
        if (SDL_GetAudioDeviceStatus(dev) == SDL_AUDIO_PAUSED) {
            platform_sleep(0.125);
            continue;
        }
        SDL_LockMutex(stream_mutex);
        vector<SoundStream*>::const_iterator it;
        for (int i = 0; i < MAX_SOUNDS; ++i) {
            SoundStream * stream = streams[i];
            if (stream == NULL)
                continue;
            if (stream->flags & SoundBase::DESTROY_STREAM) {
                delete stream;
                streams[i] = NULL;
                continue;
            }
            stream->update();
        }
        SDL_UnlockMutex(stream_mutex);

#ifdef USE_THREAD_PRELOAD
        SDL_LockMutex(stream_cond_mutex);
        SDL_CondWaitTimeout(stream_cond, stream_cond_mutex, 125);
        SDL_UnlockMutex(stream_cond_mutex);
#else
        platform_sleep(0.125);
#endif
    }
}

int AudioDevice::_stream_update(void * data)
{
    ((AudioDevice*)data)->stream_update();
    return 1;
}

void AudioDevice::add_stream(SoundStream * stream)
{
    for (int i = 0; i < MAX_SOUNDS; ++i) {
        if (streams[i] != NULL)
            continue;
        streams[i] = stream;
        break;
    }
}

void AudioDevice::remove_stream(SoundStream * stream)
{
    for (int i = 0; i < MAX_SOUNDS; ++i) {
        if (streams[i] != stream)
            continue;
        streams[i] = NULL;
        break;
    }
}

// Sample implementation

Sample::Sample(FSFile & fp, Media::AudioType type, size_t size)
{
    SoundDecoder * file = create_decoder(fp, type, size);
    channels = file->channels;
    sample_rate = file->sample_rate;
    samples = file->get_samples();
    signed short * scratch = get_scratch(samples);
    samples = file->read(scratch, samples);
    data = new float[samples];
    to_float(scratch, data, samples);
    delete file;
}

Sample::Sample(unsigned char * in_data, Media::AudioType type, size_t size)
{
    SoundDecoder * file = create_decoder(in_data, type, size);
    channels = file->channels;
    sample_rate = file->sample_rate;
	samples = file->get_samples();
	signed short * scratch = get_scratch(samples);
	samples = file->read(scratch, samples);
	data = new float[samples];
	to_float(scratch, data, samples);
    delete file;
}

Sample::~Sample()
{
    delete[] data;
    std::cout << "Cannot destroy sample" << std::endl;
}

} // namespace ChowdrenAudio
