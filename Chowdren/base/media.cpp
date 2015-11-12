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

// #define TEST_SDL_AUDIO

#ifdef CHOWDREN_IS_DESKTOP
#if defined(CHOWDREN_IS_ANDROID) || defined(TEST_SDL_AUDIO)
#include "desktop/audiosdl.h"
#else
#include "desktop/audio.h"
#endif
#else
#include "audio.h"
#endif

#include "math.h"
#include <algorithm>
#include "fileio.h"
#include "path.h"
#include "media.h"
#include "datastream.h"

inline double clamp_sound(double val)
{
    return std::max(0.0, std::min(val, 100.0));
}

Media::AudioType get_audio_type(const std::string & filename)
{
    Media::AudioType type;
    if (get_path_ext(filename) == "wav")
        type = Media::WAV;
    else
        type = Media::OGG;
    return type;
}

class SoundData
{
public:
    unsigned int id;

    SoundData(unsigned int id)
    : id(id)
    {
    }

    virtual void load(ChowdrenAudio::SoundBase ** source) {}
    virtual ~SoundData() {}
};

class SoundFile : public SoundData
{
public:
    Media::AudioType type;
    std::string filename;
    size_t size;

    SoundFile(unsigned int id, const std::string & filename,
              Media::AudioType type, size_t size)
    : SoundData(id), type(type), filename(filename), size(size)
    {
    }

    void load(ChowdrenAudio::SoundBase ** source)
    {
        *source = new ChowdrenAudio::SoundStream(filename, type, size);
    }
};

class SoundCache : public SoundData
{
public:
    Media::AudioType type;
    size_t offset;
    size_t size;

    SoundCache(unsigned int id, size_t offset, Media::AudioType type,
               size_t size)
    : SoundData(id), type(type), offset(offset), size(size)
    {
    }

    void load(ChowdrenAudio::SoundBase ** source)
    {
        *source = new ChowdrenAudio::SoundStream(offset, type, size);
    }
};

class SoundMemory : public SoundData
{
public:
    ChowdrenAudio::Sample * buffer;

    SoundMemory(unsigned int id, FSFile & fp, Media::AudioType type,
                size_t size)
    : SoundData(id), buffer(NULL)
    {
        // load immediately
        buffer = new ChowdrenAudio::Sample(fp, type, size);
    }

    SoundMemory(unsigned int id, unsigned char * data, Media::AudioType type,
                size_t size)
    : SoundData(id), buffer(NULL)
    {
        buffer = new ChowdrenAudio::Sample(data, type, size);
    }

    void load(ChowdrenAudio::SoundBase ** source)
    {
        *source = new ChowdrenAudio::Sound(*buffer);
    }

    ~SoundMemory()
    {
        delete buffer;
    }
};

// Channel

Channel::Channel()
: locked(false), volume(100), frequency(0), pan(0), sound(NULL)
{

}

void Channel::play(SoundData * data, int loop)
{
    stop();
    id = data->id;
    data->load(&sound);
    if (sound == NULL) {
        std::cout << "Ignored play" << std::endl;
        return;
    }
    set_volume(volume);
    set_pan(pan);
    if (frequency != 0)
        set_frequency(frequency);
    sound->set_loop(loop == 0);
    if (loop > 1)
        std::cout << "Invalid number of loops (" << loop << ")" << std::endl;
    sound->play();
}

void Channel::resume()
{
    if (is_invalid())
        return;
    if (sound->get_status() != ChowdrenAudio::SoundBase::Paused)
        return;
    sound->play();
}

void Channel::pause()
{
    if (is_invalid())
        return;
    if (sound->get_status() != ChowdrenAudio::SoundBase::Playing)
        return;
    sound->pause();
}

void Channel::stop()
{
    if (sound == NULL)
        return;
    sound->destroy();
    sound = NULL;
}

void Channel::set_volume(double value)
{
    volume = clamp_sound(value);
    if (is_invalid())
        return;
    sound->set_volume(volume / 100.0);
}

void Channel::set_frequency(double value)
{
    frequency = value;
    if (is_invalid())
        return;
    sound->set_frequency(value);
}

double Channel::get_frequency()
{
    if (frequency != 0)
        return frequency;
    if (is_invalid())
        return 0.0;
    return sound->get_sample_rate();
}

void Channel::set_position(double value)
{
    if (is_invalid())
        return;
    sound->set_playing_offset(value / 1000.0);
}

double Channel::get_position()
{
    if (is_invalid())
        return 0.0;
    return sound->get_playing_offset() * 1000.0;
}

double Channel::get_duration()
{
    if (is_invalid())
        return 0.0;
    return sound->get_duration() * 1000.0;
}

void Channel::set_pan(double value)
{
    pan = value;
    if (is_invalid())
        return;
    value /= 100;
    if (value > 1.0)
        value = 1.0;
    else if (value < -1.0)
        value = -1.0;
    sound->set_pan(value);
}

bool Channel::is_invalid()
{
    return sound == NULL || sound->closed;
}

bool Channel::is_stopped()
{
    if (is_invalid())
        return true;
    return sound->get_status() == ChowdrenAudio::SoundBase::Stopped;
}

// Media

static unsigned char * startup_data;
static unsigned int startup_size;

#ifdef USE_FILE_PRELOAD
#include "audiopreload.h"
#endif

void Media::init()
{
    ChowdrenAudio::open_audio();

#ifdef USE_FILE_PRELOAD
    preload_audio();
#endif

    double start_time = platform_get_time();

    AssetFile fp;
    fp.open();

#ifdef CHOWDREN_IS_WIIU
    // experimental code to load faster
    unsigned int start = AssetFile::get_offset(0, AssetFile::SOUND_DATA);
    unsigned int size = AssetFile::get_size(AssetFile::SOUND_DATA);
    startup_data = new unsigned char[size];
    startup_size = size;
    fp.seek(start);
	fp.read(startup_data, size);

    for (int i = 0; i < SOUND_COUNT; i++) {
        add_cache(i);
    }

	delete[] startup_data;
#else
    for (int i = 0; i < SOUND_COUNT; i++) {
        fp.set_item(i, AssetFile::SOUND_DATA);
        add_cache(i, fp);
    }
#endif

    std::cout << "Sound bank took " << (platform_get_time() - start_time)
        << std::endl;
}

void Media::stop()
{
    stop_samples();
    ChowdrenAudio::close_audio();
}

void Media::play(SoundData * data, int channel, int loop)
{
    if (channel == -1) {
        Channel * channelp;
        for (channel = 0; channel < 32; channel++) {
            channelp = &channels[channel];
            if (channelp->is_stopped() && !channelp->locked)
                break;
        }
        if (channel == 32) {
            for (channel = 0; channel < 32; channel++) {
                channelp = &channels[channel];
                if (!channelp->locked)
                    break;

            }
            if (channel == 32)
                return;
        }
        // unspecified channel does not inherit settings
        channelp->volume = 100;
        channelp->frequency = 0;
        channelp->pan = 0;
    }
    channels[channel].play(data, loop);
}

void Media::play(const std::string & in, int channel, int loop)
{
    std::string filename = convert_path(in);
    size_t size;
#ifdef USE_FILE_PRELOAD
    ChowdrenAudio::AudioPreload * preload;
    preload = ChowdrenAudio::get_audio_preload(filename);
    if (preload != NULL) {
        size = preload->size;
    } else
#endif
    {
        size = platform_get_file_size(filename.c_str());
    }
    if (size <= 0) {
        std::cout << "Audio file does not exist: " << filename
            << std::endl;
        return;
    }
    AudioType type = get_audio_type(filename);
    SoundFile data(INVALID_ASSET_ID, filename, type, size);
    play(&data, channel, loop);
}

void Media::play_id(unsigned int id, int channel, int loop)
{
    play(sounds[id], channel, loop);
}

void Media::lock(unsigned int channel)
{
    if (!is_channel_valid(channel))
        return;
    channels[channel].locked = true;
}

void Media::unlock(unsigned int channel)
{
    if (!is_channel_valid(channel))
        return;
    channels[channel].locked = false;
}

void Media::set_channel_volume(unsigned int channel, double volume)
{
    if (!is_channel_valid(channel))
        return;
    channels[channel].set_volume(volume);
}

void Media::set_channel_frequency(unsigned int channel, double freq)
{
    if (!is_channel_valid(channel))
        return;
    channels[channel].set_frequency(freq);
}

void Media::set_channel_pan(unsigned int channel, double pan)
{
    if (!is_channel_valid(channel))
        return;
    channels[channel].set_pan(pan);
}

void Media::stop_channel(unsigned int channel)
{
    if (!is_channel_valid(channel))
        return;
    channels[channel].stop();
}

void Media::resume_channel(unsigned int channel)
{
    if (!is_channel_valid(channel))
        return;
    channels[channel].resume();
}

void Media::pause_channel(unsigned int channel)
{
    if (!is_channel_valid(channel))
        return;
    channels[channel].pause();
}

Channel * Media::get_sample(unsigned int id)
{
    if (id == INVALID_ASSET_ID)
        return NULL;
    for (int i = 0; i < 32; i++) {
        if (channels[i].id != id)
            continue;
        return &channels[i];
    }
    return NULL;
}

void Media::set_sample_volume(unsigned int id, double volume)
{
    Channel * channel = get_sample(id);
    if (channel == NULL)
        return;
    channel->set_volume(volume);
}

void Media::set_sample_pan(unsigned int id, double pan)
{
    Channel * channel = get_sample(id);
    if (channel == NULL)
        return;
    channel->set_pan(pan);
}

void Media::set_sample_position(unsigned int id, double pos)
{
    Channel * channel = get_sample(id);
    if (channel == NULL)
        return;
    channel->set_position(pos);
}

void Media::set_sample_frequency(unsigned int id, double freq)
{
    Channel * channel = get_sample(id);
    if (channel == NULL)
        return;
    channel->set_frequency(freq);
}

double Media::get_sample_position(unsigned int id)
{
    Channel * channel = get_sample(id);
    if (channel == NULL)
        return 0.0;
    return channel->get_position();
}

double Media::get_sample_volume(unsigned int id)
{
    Channel * channel = get_sample(id);
    if (channel == NULL)
        return 0.0;
    return channel->volume;
}

double Media::get_sample_duration(unsigned int id)
{
    Channel * channel = get_sample(id);
    if (channel == NULL)
        return 0.0;
    return channel->get_duration();
}

void Media::stop_sample(unsigned int id)
{
    Channel * channel = get_sample(id);
    if (channel == NULL)
        return;
    channel->stop();
}

void Media::stop_samples()
{
    for (int i = 0; i < 32; i++) {
        stop_channel(i);
    }
}

void Media::pause_samples()
{
    for (int i = 0; i < 32; i++) {
        pause_channel(i);
    }
}

void Media::resume_samples()
{
    for (int i = 0; i < 32; i++) {
        resume_channel(i);
    }
}

double Media::get_channel_position(unsigned int channel)
{
    if (!is_channel_valid(channel))
        return 0.0;
    return channels[channel].get_position();
}

double Media::get_channel_frequency(unsigned int channel)
{
    if (!is_channel_valid(channel))
        return 0.0;
    return channels[channel].get_frequency();
}

void Media::set_channel_position(unsigned int channel, double pos)
{
    if (!is_channel_valid(channel))
        return;
    return channels[channel].set_position(pos);
}

double Media::get_channel_duration(unsigned int channel)
{
    if (!is_channel_valid(channel))
        return 0.0;
    return channels[channel].get_duration();
}

double Media::get_channel_volume(unsigned int channel)
{
    if (!is_channel_valid(channel))
        return 0.0;
    return channels[channel].volume;
}

double Media::get_channel_pan(unsigned int channel)
{
    if (!is_channel_valid(channel))
        return 0.0;
    return channels[channel].pan;
}

bool Media::is_channel_playing(unsigned int channel)
{
    if (!is_channel_valid(channel))
        return false;
    return !channels[channel].is_stopped();
}

bool Media::is_sample_playing(unsigned int id)
{
    for (int i = 0; i < 32; i++) {
        if (channels[i].is_stopped())
            continue;
        if (channels[i].id == id)
            return true;
    }
    return false;
}

bool Media::is_channel_valid(unsigned int channel)
{
    return channel < 32;
}

#if defined(CHOWDREN_IS_DESKTOP)
#define OGG_STREAM_THRESHOLD_MB 0.5
#elif defined(CHOWDREN_IS_3DS) || defined(CHOWDREN_IS_ANDROID)
#define OGG_STREAM_THRESHOLD_MB 0.1
#else
#define OGG_STREAM_THRESHOLD_MB 0.75
#endif

#define WAV_STREAM_THRESHOLD_MB 0.2

#define OGG_STREAM_THRESHOLD (OGG_STREAM_THRESHOLD_MB * 1024 * 1024)
#define WAV_STREAM_THRESHOLD (WAV_STREAM_THRESHOLD_MB * 1024 * 1024)

void Media::add_file(unsigned int id, const std::string & fn)
{
    std::string filename = convert_path(fn);
    AudioType type = get_audio_type(filename);
    size_t size = platform_get_file_size(filename.c_str());

    bool is_wav = type == WAV;
    SoundData * data;
    if ((is_wav && size <= WAV_STREAM_THRESHOLD) ||
        (!is_wav && size <= OGG_STREAM_THRESHOLD))
    {
        FSFile fp(filename.c_str(), "r");
        data = new SoundMemory(id, fp, type, size);
    } else {
        data = new SoundFile(id, filename, type, size);
    }
    sounds[id] = data;
}

void Media::add_cache(unsigned int id)
{
    ArrayStream stream((char*)startup_data, startup_size);
    unsigned int start = AssetFile::get_offset(0, AssetFile::SOUND_DATA);
    unsigned int offset = AssetFile::get_offset(id, AssetFile::SOUND_DATA);
    unsigned int stream_offset = offset - start;
    stream.seek(stream_offset);

    AudioType type = (AudioType)stream.read_uint32();
    if (type == NONE)
        return;
    unsigned int size = stream.read_uint32();

    bool is_wav = type == WAV;
    SoundData * data;
    if ((is_wav && size <= WAV_STREAM_THRESHOLD) ||
        (!is_wav && size <= OGG_STREAM_THRESHOLD))
    {
        data = new SoundMemory(id, &startup_data[stream.pos], type, size);
    } else {
        unsigned int data_offset = stream.pos - stream_offset;
        data = new SoundCache(id, offset + data_offset, type, size);
    }
    media.sounds[id] = data;
}

void Media::add_cache(unsigned int id, FSFile & fp)
{
    FileStream stream(fp);
    AudioType type = (AudioType)stream.read_uint32();
    if (type == NONE)
        return;
    unsigned int size = stream.read_uint32();
    size_t pos = fp.tell();

    bool is_wav = type == WAV;
    SoundData * data;
    if ((is_wav && size <= WAV_STREAM_THRESHOLD) ||
        (!is_wav && size <= OGG_STREAM_THRESHOLD))
    {
#ifdef CHOWDREN_IS_3DS
        data = new SoundMemory(id, fp, type, size);
#else
        unsigned char * sound_data = new unsigned char[size];
        fp.read(sound_data, size); 
        data = new SoundMemory(id, sound_data, type, size);
        delete[] sound_data;
#endif
    } else {
        data = new SoundCache(id, fp.tell(), type, size);
    }
    media.sounds[id] = data;
    fp.seek(pos + size);
}

double Media::get_main_volume()
{
    return ChowdrenAudio::Listener::get_volume() * 100.0;
}

void Media::set_main_volume(double volume)
{
    ChowdrenAudio::Listener::set_volume(clamp_sound(volume) / 100.0);
}

Media media;
