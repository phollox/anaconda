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

#include "chowstring.h"
#include <iostream>
#include <android/log.h>
#include "platform.h"
#include <SDL_rwops.h>

namespace ChowdrenAudio
{
    void pause_audio();
    void resume_audio();
}

static chowstring current_log_line;

class LogBuffer : public std::streambuf
{
public:
    LogBuffer()
    {
        // no buffering, overflow on every char
        setp(0, 0);
    }

    virtual int_type overflow(int_type c = traits_type::eof())
    {
        if (c == '\n') {
            __android_log_print(ANDROID_LOG_INFO, "Chowdren",
                                current_log_line.c_str());
            current_log_line.clear();
        } else {
            current_log_line += c;
        }
        return c;
    }
};

#ifdef USE_ASSET_MANAGER
extern "C" {
#undef __cplusplus
#include <jni.h>
#define __cplusplus
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/configuration.h>
}
#include <SDL_system.h>
static jobject java_asset_manager;
AAssetManager * global_asset_manager;
static chowstring internal_path;
static chowstring android_language("English");

extern "C" JNIEnv *Android_JNI_GetEnv(void);

void init_asset_manager()
{
    jmethodID mid;

    JNIEnv *env = Android_JNI_GetEnv();
    const int capacity = 16;
    (*env)->PushLocalFrame(env, capacity);

    jclass mActivityClass = (*env)->FindClass(env,
                                              "org/libsdl/app/SDLActivity");

    /* context = SDLActivity.getContext(); */
    mid = (*env)->GetStaticMethodID(env, mActivityClass,
            "getContext","()Landroid/content/Context;");
    jobject context = (*env)->CallStaticObjectMethod(env, mActivityClass,
                                                     mid);

    /* assetManager = context.getAssets(); */
    mid = (*env)->GetMethodID(env, (*env)->GetObjectClass(env, context),
            "getAssets", "()Landroid/content/res/AssetManager;");
    jobject asset_manager = (*env)->CallObjectMethod(env, context, mid);
    java_asset_manager = (*env)->NewGlobalRef(env, asset_manager);
    global_asset_manager = AAssetManager_fromJava(env, java_asset_manager);

    (*env)->PopLocalFrame(env, NULL);

    internal_path = SDL_AndroidGetInternalStoragePath();

    AConfiguration * config = AConfiguration_new();
    AConfiguration_fromAssetManager(config, global_asset_manager);
    char lang[2];
    AConfiguration_getLanguage(config, lang);
    AConfiguration_delete(config);
    #define MAKE_LANG(a, b) (a | (b << 8))
    unsigned int lang_id = MAKE_LANG(lang[0], lang[1]);

    switch (lang_id) {
        case MAKE_LANG('e', 'n'):
            android_language = "English";
            break;
        case MAKE_LANG('i', 't'):
            android_language = "Italian";
            break;
        case MAKE_LANG('e', 's'):
            android_language = "Spanish";
            break;
        case MAKE_LANG('d', 'e'):
            android_language = "German";
            break;
        case MAKE_LANG('f', 'r'):
            android_language = "French";
            break;
        default:
            android_language = "English";
            break;
    }

    #undef MAKE_LANG
}
#endif

void platform_init_android()
{
    static LogBuffer ob;
    std::streambuf * cout_sb = std::cout.rdbuf(&ob);
    std::streambuf * cerr_sb = std::cerr.rdbuf(&ob);
    __android_log_print(ANDROID_LOG_INFO, "Chowdren", "Initialized logbuffer");
#ifdef USE_ASSET_MANAGER
    init_asset_manager();
#endif
}

const chowstring & platform_get_language()
{
    return android_language;
}

#ifdef USE_ASSET_MANAGER

void platform_walk_folder(const chowstring & in_path,
                          FolderCallback & callback)
{
}

size_t platform_get_file_size(const char * filename)
{
    FSFile fp(filename, "r");
    if (!fp.is_open())
        return 0;
    return fp.get_size();
}

bool platform_is_directory(const chowstring & value)
{
    return false;
}

bool platform_is_file(const chowstring & value)
{
    FSFile fp(value.c_str(), "r");
    if (!fp.is_open())
        return 0;
    return fp.get_size();
}

bool platform_path_exists(const chowstring & value)
{
    return platform_is_file(value);
}

void platform_create_directories(const chowstring & value)
{
}

#else

void platform_walk_folder(const chowstring & in_path,
                          FolderCallback & callback)
{
}

size_t platform_get_file_size(const char * filename)
{
    chowstring path = convert_path(filename);
    SDL_RWops * rw = SDL_RWFromFile(path.c_str(), "rb");
    if (rw == NULL)
        return 0;
    size_t size = SDL_RWsize(rw);
    SDL_RWclose(rw);
    return size;
}

bool platform_is_directory(const chowstring & value)
{
    return false;
}

bool platform_is_file(const chowstring & value)
{
    chowstring path = convert_path(value);
    SDL_RWops * rw = SDL_RWFromFile(path.c_str(), "rb");
    if (rw != NULL) {
        SDL_RWclose(rw);
        return true;
    }
    return false;
}

bool platform_path_exists(const chowstring & value)
{
    return platform_is_file(value);
}

void platform_create_directories(const chowstring & value)
{
}
#endif

const chowstring & platform_get_appdata_dir()
{
    static chowstring dir(".");
    return dir;
}

#ifdef CHOWDREN_USE_GAMECIRCLE

#include "AchievementsClientInterface.h"

void platform_unlock_achievement(const chowstring & name)
{
    AmazonGames::AchievementsClientInterface::updateProgress(
        name.c_str(), 100.0f, 0);
}

#else

void platform_unlock_achievement(const chowstring & name)
{
}

#endif

#include "../desktop/platform.cpp"
