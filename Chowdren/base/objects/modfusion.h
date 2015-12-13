#ifndef CHOWDREN_MODFUSION_H
#define CHOWDREN_MODFUSION_H

#include "chowstring.h"

class ModFusion
{
public:
    static void set_volume(int track, int volume);
    static void load_track(int cache_id, int track);
    static void play(int track);
    static void stop(int track);
    static void stop_fade(int track, int ms);
    static void load_wgm(int wgm_id, int cache_id);
    static void load_wgm(const chowstring & filename);
    static void clear_cache();
    static bool is_playing(int track);
    static int get_volume(int track);
    static void enable_loop(int track);
    static void disable_loop(int track);
    static void update();
};

#endif // CHOWDREN_MODFUSION_H
