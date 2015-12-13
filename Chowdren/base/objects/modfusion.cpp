#include "modfusion.h"
#include "media.h"
#include <iostream>
#include "chowstring.h"
#include "stringcommon.h"
#include "manager.h"

static int global_cache[16];
static int track_volume = -1;
static int global_volume = 0;
static int global_track_wgm = 0;
static float fadeout_value = 0.0f;
static float fadeout_volume = 0.0f;

#define CHANNEL_ID 31

#ifdef CHOWDREN_IS_3DS
#define MOD_PATH "./mus/"
#define MOD_EXT ".mus"
#else
#define MOD_PATH "./ogg/"
#define MOD_EXT ".ogg"
#endif

void ModFusion::set_volume(int track, int volume)
{
	if (track != 0)
		return;
    if (track_volume == -1 && volume != 0)
        track_volume = volume;
    global_volume = volume;
    int vol = (global_volume * 100) / track_volume;
    std::cout << "Set volume: " << track << " " << volume << " " << vol << std::endl;
    media.set_channel_volume(CHANNEL_ID, vol);
}

void ModFusion::load_track(int cache_id, int track)
{
    std::cout << "Load track: " << cache_id << " " << track << std::endl;
    global_track_wgm = global_cache[cache_id];
}

void ModFusion::play(int track)
{
    fadeout_value = 0.0f;
    media.lock(CHANNEL_ID);
    chowstring path = MOD_PATH + number_to_string(global_track_wgm) + MOD_EXT;
    media.play(path, CHANNEL_ID, 0);
    track_volume = -1;
    std::cout << "Play mod: " << track << " " << global_track_wgm << std::endl;
}

void ModFusion::stop(int track)
{
    fadeout_value = 0.0f;
    media.stop_channel(CHANNEL_ID);
    std::cout << "Stop mod: " << track << std::endl;
}

void ModFusion::stop_fade(int track, int ms)
{
    if (ms == 0) {
        stop(track);
        return;
    }
    fadeout_volume = global_volume;
    fadeout_value = fadeout_volume / (ms / 1000.0f);
    std::cout << "Stop mod with fade: " << track << " " << ms << std::endl;
}

void ModFusion::load_wgm(int wgm_id, int cache_id)
{
    std::cout << "Load WGM: " << wgm_id << " " << cache_id << std::endl;
    global_cache[cache_id] = wgm_id;
}

void ModFusion::load_wgm(const chowstring & filename)
{
    std::cout << "Load WGM file: " << filename << std::endl;
}

void ModFusion::clear_cache()
{
    std::cout << "Clear mod cache" << std::endl;
}

void ModFusion::enable_loop(int track)
{
    std::cout << "Enable loop: " << track << std::endl;
}

void ModFusion::disable_loop(int track)
{
    std::cout << "Disable loop: " << track << std::endl;
}

bool ModFusion::is_playing(int track)
{
    return media.is_channel_playing(CHANNEL_ID);
}

int ModFusion::get_volume(int track)
{
    return global_volume;
}

void ModFusion::update()
{
    if (fadeout_value == 0.0f)
        return;
    fadeout_volume -= fadeout_value * manager.dt;
    if (fadeout_volume <= 0.0f) {
        fadeout_value = fadeout_volume = 0.0f;
        media.stop_channel(CHANNEL_ID);
        set_volume(0, global_volume);
    } else {
        set_volume(0, fadeout_volume);
    }
}
