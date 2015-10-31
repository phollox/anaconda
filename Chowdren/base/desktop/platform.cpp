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

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#include <io.h>
#include <fcntl.h>
#else
#include <sys/time.h>
#endif

#include "chowconfig.h"
#include "platform.h"
#include "include_gl.h"
#include "manager.h"
#include "mathcommon.h"
#include "fbo.h"
#include <iostream>
#include "platform.h"
#include <SDL.h>
// SDL_syswm.h defines WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#include <SDL_syswm.h>
#include <time.h>
#include <boost/cstdint.hpp>
#include "path.h"
#include "render.h"

#ifdef _WIN32
#include <windows.h>
#include "shlobj.h"
#include <shellapi.h>
#elif __APPLE__
#include <CoreServices/CoreServices.h>
#include <limits.h>
#include <unistd.h>
#include <CoreFoundation/CoreFoundation.h>
#include <sys/param.h> // For MAXPATHLEN
#include <dirent.h>
#elif defined(__linux) || defined(CHOWDREN_IS_EMSCRIPTEN)
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <dirent.h>
#endif

enum ScaleType
{
    BEST_FIT = 0,
    BEST_FIT_INT = 1,
    EXACT_FIT = 2
};

static Framebuffer screen_fbo;
static SDL_Window * global_window = NULL;
static int global_window_id;

#ifdef CHOWDREN_USE_D3D
static D3DPRESENT_PARAMETERS pparams;
static int device_display_index = -1;
static bool d3d_do_reset = false;
#else
static SDL_GLContext global_context = NULL;
#endif

static bool is_fullscreen = false;
static int fullscreen_width = -1;
static int fullscreen_height = -1;

#ifdef CHOWDREN_DEFAULT_SCALE
static int windowed_width = WINDOW_WIDTH * CHOWDREN_DEFAULT_SCALE;
static int windowed_height = WINDOW_HEIGHT * CHOWDREN_DEFAULT_SCALE;
#else
static int windowed_width = WINDOW_WIDTH;
static int windowed_height = WINDOW_HEIGHT;
#endif
static bool hide_cursor = false;
static bool has_closed = false;
static Uint64 start_time;
int scale_type = BEST_FIT;

static int draw_x_size = 0;
static int draw_y_size = 0;
static int draw_x_off = 0;
static int draw_y_off = 0;

#ifdef CHOWDREN_USE_GL
// opengl function pointers
PFNGLBLENDEQUATIONSEPARATEEXTPROC __glBlendEquationSeparateEXT;
PFNGLBLENDEQUATIONEXTPROC __glBlendEquationEXT;
PFNGLBLENDFUNCSEPARATEEXTPROC __glBlendFuncSeparateEXT;
PFNGLACTIVETEXTUREARBPROC __glActiveTextureARB;
PFNGLCLIENTACTIVETEXTUREARBPROC __glClientActiveTextureARB;
PFNGLGENFRAMEBUFFERSEXTPROC __glGenFramebuffersEXT;
PFNGLDELETEFRAMEBUFFERSEXTPROC __glDeleteFramebuffersEXT;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC __glFramebufferTexture2DEXT;
PFNGLBINDFRAMEBUFFEREXTPROC __glBindFramebufferEXT;

PFNGLUSEPROGRAMOBJECTARBPROC __glUseProgramObjectARB;
PFNGLDETACHOBJECTARBPROC __glDetachObjectARB;
PFNGLGETINFOLOGARBPROC __glGetInfoLogARB;
PFNGLGETOBJECTPARAMETERIVARBPROC __glGetObjectParameterivARB;
PFNGLLINKPROGRAMARBPROC __glLinkProgramARB;
PFNGLCREATEPROGRAMOBJECTARBPROC __glCreateProgramObjectARB;
PFNGLATTACHOBJECTARBPROC __glAttachObjectARB;
PFNGLCOMPILESHADERARBPROC __glCompileShaderARB;
PFNGLSHADERSOURCEARBPROC __glShaderSourceARB;
PFNGLCREATESHADEROBJECTARBPROC __glCreateShaderObjectARB;
PFNGLUNIFORM1IARBPROC __glUniform1iARB;
PFNGLUNIFORM2FARBPROC __glUniform2fARB;
PFNGLUNIFORM1FARBPROC __glUniform1fARB;
PFNGLUNIFORM4FARBPROC __glUniform4fARB;
PFNGLGETUNIFORMLOCATIONARBPROC __glGetUniformLocationARB;
#endif

#ifndef CHOWDREN_USE_D3D
void set_gl_state();

static bool check_opengl_extension(const char * name)
{
    if (SDL_GL_ExtensionSupported(name) == SDL_TRUE)
        return true;
    std::string message;
    message += "OpenGL extension '";
    message += name;
    message += "' not supported.";
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "OpenGL error",
                             message.c_str(), NULL);
    return false;
}

const char * extensions[] = {
#ifdef CHOWDREN_USE_GL
    "GL_EXT_framebuffer_object",
    "GL_ARB_shader_objects",
    "GL_ARB_vertex_shader",
    "GL_ARB_fragment_shader",
    "GL_ARB_texture_non_power_of_two",
#endif
    NULL
};

static bool check_opengl_extensions()
{
    for (int i = 0; extensions[i] != NULL; i++)
        if (!check_opengl_extension(extensions[i]))
            return false;
    return true;
}
#endif

#ifdef __APPLE__
static bool has_cmd = false;
#endif

static void on_key(SDL_KeyboardEvent & e)
{
    if (e.repeat != 0)
        return;
#ifdef __APPLE__
    if (SDL_GetModState() & (KMOD_LGUI | KMOD_RGUI))
        has_cmd = true;
    else
        has_cmd = false;
#endif
    bool state = e.state == SDL_PRESSED;
    int key = e.keysym.sym;

    if (state && key == SDLK_RETURN && (SDL_GetModState() & KMOD_ALT)) {
        manager.set_window(!manager.fullscreen);
        return;
    }

    if (state && key == SDLK_F2) {
        manager.frame->restart();
    }

#ifdef CHOWDREN_USE_EDITOBJ
    if (state && key == SDLK_v && (SDL_GetModState() & KMOD_CTRL) &&
        SDL_HasClipboardText())
    {
        char * text = SDL_GetClipboardText();
        manager.input += text;
        SDL_free(text);
        return;
    }
#endif

    manager.on_key(key, state);
}

static void on_mouse(SDL_MouseButtonEvent & e)
{
    int button = e.button;
#ifdef __APPLE__
    if (has_cmd && button == SDL_BUTTON_LEFT)
        button = SDL_BUTTON_RIGHT;
#endif
    manager.on_mouse(button, e.state == SDL_PRESSED);
}

void init_joystick();
void update_joystick();
void add_joystick(int device);
void remove_joystick(int instance);
void on_joystick_button(int instance, int button, bool state);
void on_controller_button(int instance, int button, bool state);
void d3d_reset(int w, int h);
void d3d_reset_state();
void d3d_set_backtex_size(int w, int h);

#ifdef __APPLE__

static void set_resources_dir()
{
    char resourcesPath[MAXPATHLEN];

    CFBundleRef bundle = CFBundleGetMainBundle();
    if (!bundle)
        return;

    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(bundle);

    CFStringRef last = CFURLCopyLastPathComponent(resourcesURL);
    if (CFStringCompare(CFSTR("Resources"), last, 0) != kCFCompareEqualTo)
    {
        CFRelease(last);
        CFRelease(resourcesURL);
        return;
    }

    CFRelease(last);

    if (!CFURLGetFileSystemRepresentation(resourcesURL,
                                          true,
                                          (UInt8*) resourcesPath,
                                          MAXPATHLEN))
    {
        CFRelease(resourcesURL);
        return;
    }

    CFRelease(resourcesURL);

    chdir(resourcesPath);
}
#endif

#ifndef NDEBUG
static void sdl_log(void *userdata, int category, SDL_LogPriority priority,
                    const char * message)
{
    std::cout << "SDL log (" << category << ", " << priority << "): "
              << message << std::endl;
}
#endif

void platform_init()
{
#ifdef CHOWDREN_IS_ANDROID
    platform_init_android();
#endif
    unsigned int flags = SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE;
    if (SDL_Init(flags) < 0) {
        std::cout << "SDL could not be initialized: " << SDL_GetError()
            << std::endl;
        return;
    }
    SDL_EventState(SDL_MOUSEMOTION, SDL_DISABLE);
    // SDL_EventState(SDL_WINDOWEVENT, SDL_DISABLE);
#ifdef __APPLE__
    set_resources_dir();
#endif
    start_time = SDL_GetPerformanceCounter();
    init_joystick();

#ifdef _WIN32
    timeBeginPeriod(1);
#endif

#ifndef NDEBUG
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_WARN);
    SDL_LogSetOutputFunction(sdl_log, NULL);
#endif
}

// Here, we check if vsync is causing the game to slow down from 60fps to lower
// than ~55fps.

static int vsync_value = -2;
static double vsync_fail_time = 0.0;
static bool disable_resize_shader = false;

void check_slowdown()
{
    int window_flags = SDL_GetWindowFlags(global_window);
    if (window_flags & (SDL_WINDOW_MINIMIZED | SDL_WINDOW_HIDDEN))
        return;

    double expected = 1.0 / manager.fps_limit.framerate;
    if (manager.fps_limit.dt - expected > 0.0025) {
        vsync_fail_time += expected;
    } else {
        vsync_fail_time = 0.0;
        return;
    }

    if (vsync_fail_time <= 2.0)
        return;

    vsync_fail_time = 0.0;

#if defined(CHOWDREN_SPECIAL_POINT_FILTER) && !defined(CHOWDREN_FORCE_FILTER)
    if (!disable_resize_shader) {
        std::cout << "Disable resize shader, too slow " << std::endl;
        disable_resize_shader = true;
        return;
    }
#endif

    if (vsync_value == 1) {
        std::cout << "Swap interval causing slowdown, turning off vsync"
            << std::endl;
        vsync_fail_time = 0.0;
        platform_set_vsync(0);
    }
}

void platform_poll_events()
{
    check_slowdown();

#ifdef CHOWDREN_USE_EDITOBJ
    manager.input.clear();
#endif

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                on_key(e.key);
                break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                on_mouse(e.button);
                break;
            case SDL_JOYDEVICEADDED:
                add_joystick(e.jdevice.which);
                break;
            case SDL_JOYDEVICEREMOVED:
                remove_joystick(e.jdevice.which);
                break;
            case SDL_JOYBUTTONDOWN:
            case SDL_JOYBUTTONUP:
                on_joystick_button(e.jbutton.which, e.jbutton.button,
                                   e.jbutton.state == SDL_PRESSED);
                break;
            case SDL_CONTROLLERBUTTONDOWN:
            case SDL_CONTROLLERBUTTONUP:
                on_controller_button(e.cbutton.which, e.cbutton.button,
                                     e.cbutton.state == SDL_PRESSED);
                break;

            case SDL_QUIT:
                has_closed = true;
                break;

            #if defined(CHOWDREN_USE_EDITOBJ) || defined(CHOWDREN_USE_GWEN)
            case SDL_TEXTINPUT:
                manager.input += e.text.text;
                break;
            #endif

            // d3d-specific
            #ifdef CHOWDREN_USE_D3D
            case SDL_WINDOWEVENT: {
                if (e.window.windowID != global_window_id)
                    break;
                if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    int w = e.window.data1;
                    int h = e.window.data2;
                    std::cout << "Size changed: " << w << " " << h << std::endl;
                    if (!is_fullscreen) {
                        windowed_width = e.window.data1;
                        windowed_height = e.window.data2;
                    }
                    if (is_fullscreen && (w != fullscreen_width ||
                                          h != fullscreen_height))
                        break;
                    d3d_do_reset = true;
                    break;
                } else if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                    int w = e.window.data1;
                    int h = e.window.data2;
                    std::cout << "Resized: " << w << " " << h << std::endl;
                }
                break;
            }
            #endif

            // android specific
            #if defined(CHOWDREN_IS_ANDROID)
            case SDL_APP_WILLENTERBACKGROUND:
            case SDL_APP_DIDENTERBACKGROUND:
                ChowdrenAudio::pause_audio();
                break;
            case SDL_APP_DIDENTERFOREGROUND:
            case SDL_APP_WILLENTERFOREGROUND:
                ChowdrenAudio::resume_audio();
                break;
            #endif

            default:
                break;
        }
    }
    update_joystick();
}

// time

double platform_get_time()
{
    Uint64 s = SDL_GetPerformanceCounter();
    s -= start_time;
    return s / double(SDL_GetPerformanceFrequency());
}

unsigned int platform_get_global_time()
{
    return time(NULL);
}

static DateTime datetime;

const DateTime & platform_get_datetime()
{
    time_t now = time(NULL);
    tm * t = localtime(&now);
    datetime.sec = t->tm_sec;
    datetime.min = t->tm_min;
    datetime.hour = t->tm_hour;
    datetime.mday = t->tm_mday;
    datetime.mon = t->tm_mon;
    datetime.year = t->tm_year;
    datetime.wday = t->tm_wday;
    datetime.yday = t->tm_yday;
    return datetime;
}

void platform_sleep(double t)
{
    if (t < 0.001)
        return;
    SDL_Delay(t * 1000.0);
}

bool platform_display_closed()
{
    if (global_window == NULL)
        return true;
    return has_closed;
}

void platform_get_mouse_pos(int * x, int * y)
{
    SDL_GetMouseState(x, y);
    *x = (*x - draw_x_off) * (float(WINDOW_WIDTH) / draw_x_size);
    *y = (*y - draw_y_off) * (float(WINDOW_HEIGHT) / draw_y_size);
}

#if !defined(NDEBUG) && !defined(CHOWDREN_USE_D3D)
// #define CHOWDREN_USE_GL_DEBUG
#endif

#ifdef CHOWDREN_USE_GL_DEBUG
static void APIENTRY on_debug_message(GLenum source, GLenum type, GLuint id,
                                      GLenum severity, GLsizei length,
                                      const GLchar * message,
                                      GLvoid * param)
{
    std::cout << "OpenGL message (" << source << " " << type << " " << id <<
        ")" << message << std::endl;
}

static void APIENTRY on_debug_message_amd(GLuint id, GLenum category,
                                          GLenum severity, GLsizei length,
                                          const GLchar* message,
                                          GLvoid* userParam)
{
    std::cout << "OpenGL message (" << id << " " << category << " "
        << severity << ")" << message << std::endl;
}
#endif

#ifdef CHOWDREN_USE_D3D
static void create_d3d_device()
{
    device_display_index = SDL_GetWindowDisplayIndex(global_window);
    SDL_Rect rect;
    SDL_GetDisplayBounds(device_display_index, &rect);
    fullscreen_width = rect.w;
    fullscreen_height = rect.h;
    int adapter_index = SDL_Direct3D9GetAdapterIndex(device_display_index);

    HRESULT hr;
    D3DDEVTYPE device_type = D3DDEVTYPE_HAL;
    DWORD device_flags = D3DCREATE_FPU_PRESERVE |
                         D3DCREATE_HARDWARE_VERTEXPROCESSING |
                         D3DCREATE_PUREDEVICE;

    hr = render_data.d3d->CreateDevice(adapter_index, device_type,
                                       pparams.hDeviceWindow, device_flags,
                                       &pparams, &render_data.device);
    if (!FAILED(hr))
        return;

    // try again with fixed back buffer count
    hr = render_data.d3d->CreateDevice(adapter_index, device_type,
                                       pparams.hDeviceWindow, device_flags,
                                       &pparams, &render_data.device);
    if (!FAILED(hr))
        return;

    // Try to create the device with mixed vertex processing.
    device_flags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
    device_flags &= ~D3DCREATE_PUREDEVICE;
    device_flags |= D3DCREATE_MIXED_VERTEXPROCESSING;

    hr = render_data.d3d->CreateDevice(adapter_index, device_type,
                                       pparams.hDeviceWindow, device_flags,
                                       &pparams, &render_data.device);
    if (!FAILED(hr))
        return;

    // try to create the device with software vertex processing.
    device_flags &= ~D3DCREATE_MIXED_VERTEXPROCESSING;
    device_flags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    hr = render_data.d3d->CreateDevice(adapter_index, device_type,
                                       pparams.hDeviceWindow, device_flags,
                                       &pparams, &render_data.device);
    if (!FAILED(hr))
        return;

    // try reference device
    device_type = D3DDEVTYPE_REF;
    hr = render_data.d3d->CreateDevice(adapter_index, device_type,
                                       pparams.hDeviceWindow, device_flags,
                                       &pparams, &render_data.device);
    if (!FAILED(hr))
        return;

    std::cout << "Could not create D3D context" << std::endl;
    exit(EXIT_FAILURE);
}

static D3DFORMAT
PixelFormatToD3DFMT(Uint32 format)
{
    switch (format) {
    case SDL_PIXELFORMAT_RGB565:
        return D3DFMT_R5G6B5;
    case SDL_PIXELFORMAT_RGB888:
        return D3DFMT_X8R8G8B8;
    case SDL_PIXELFORMAT_ARGB8888:
        return D3DFMT_A8R8G8B8;
    case SDL_PIXELFORMAT_YV12:
    case SDL_PIXELFORMAT_IYUV:
        return D3DFMT_L8;
    default:
        return D3DFMT_UNKNOWN;
    }
}

static bool last_fullscreen = false;
static int last_w = 0;
static int last_h = 0;

void d3d_set_window(int w, int h)
{
    last_fullscreen = false;
    pparams.BackBufferWidth = w;
    pparams.BackBufferHeight = h;
    pparams.Windowed = TRUE;
    pparams.FullScreen_RefreshRateInHz = 0;
    pparams.BackBufferFormat = D3DFMT_UNKNOWN;
}

void d3d_reset(bool last_failed)
{
    int w, h;
    if (is_fullscreen) {
        w = fullscreen_width;
        h = fullscreen_height;
        std::cout << "Is fullscreen, size: " << w << " " << h << std::endl;
    } else {
        w = windowed_width;
        h = windowed_height;
        std::cout << "Window, size: " << w << " " << h << std::endl;
    }

    // no need for reset
    if (!last_failed && last_fullscreen == is_fullscreen
        && last_w == w && last_h == h)
        return;

    std::cout << "Reset D3D device: " << w << " " << h << " " << is_fullscreen
        << std::endl;

    pparams.BackBufferWidth = w;
    pparams.BackBufferHeight = h;

    int display = SDL_GetWindowDisplayIndex(global_window);

#ifdef CHOWDREN_NO_EXCLUSIVE_MODE
    if (false) {
#else
    if (is_fullscreen && display == device_display_index) {
#endif
        SDL_DisplayMode mode;
        SDL_GetDesktopDisplayMode(display, &mode);
        pparams.Windowed = FALSE;
        pparams.FullScreen_RefreshRateInHz = mode.refresh_rate;
        pparams.BackBufferFormat = PixelFormatToD3DFMT(mode.format);
    } else {
        pparams.Windowed = TRUE;
        pparams.FullScreen_RefreshRateInHz = 0;
        pparams.BackBufferFormat = D3DFMT_UNKNOWN;
    }

    for (int i = 0; i < Framebuffer::MAX_FBO; ++i) {
        Framebuffer * fbo = Framebuffer::fbos[i];
        if (fbo == NULL || fbo->fbo == NULL)
            continue;
        fbo->fbo->Release();
        fbo->fbo = NULL;
        TextureData & t = render_data.textures[fbo->tex];
        t.texture->Release();
    }
  
    render_data.default_target->Release();
    render_data.default_target = NULL;
    render_data.textures[render_data.back_tex].texture->Release();
    render_data.textures[render_data.back_tex].texture = NULL;

    last_fullscreen = is_fullscreen;
    last_w = w;
    last_h = h;

    if (is_fullscreen && last_failed) {
        std::cout << "Last failed, set windowed" << std::endl;
        d3d_set_window(w, h);
    }

    if (is_fullscreen) {
        SDL_SetWindowFullscreen(global_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else {
        SDL_SetWindowFullscreen(global_window, 0);
    }

    HRESULT hr = render_data.device->Reset(&pparams);
    if (FAILED(hr)) {
#ifndef NDEBUG
        std::cout << "Failed reset: " << hr << std::endl;
#endif
        if (is_fullscreen) {
            std::cout << "Failing fullscreen due to device lost" << std::endl;
            d3d_set_window(w, h);
            hr = render_data.device->Reset(&pparams);
        }

        while (FAILED(hr)) {
            while (true) {
                HRESULT status = render_data.device->TestCooperativeLevel();
                if (status == D3DERR_DEVICENOTRESET)
                    break;
                platform_sleep(0.2);
            }
            hr = render_data.device->Reset(&pparams);
#ifndef NDEBUG
            std::cout << "Trying new reset: " << hr << std::endl;
#endif
        }
    }

    for (int i = 0; i < Framebuffer::MAX_FBO; ++i) {
        Framebuffer * fbo = Framebuffer::fbos[i];
        if (fbo == NULL)
            continue;
        fbo->init(fbo->w, fbo->h);
    }

    render_data.device->GetRenderTarget(0, &render_data.default_target);

    d3d_reset_state();

    render_data.backtex_width = render_data.backtex_height = 0;
    d3d_set_backtex_size(1, 1);

#ifdef CHOWDREN_PASTE_CACHE
    vector<Layer>::iterator it;
    for (it = manager.frame->layers.begin();
         it != manager.frame->layers.end();
         ++it)
    {
        Background * back = it->back;
        if (back == NULL)
            continue;
        for (int y = 0; y < back->cache_h; ++y)
        for (int x = 0; x < back->cache_w; ++x) {
            int i = y * back->cache_w + x;
            back->cache[i].dirty = true;
        }
    }
#endif
}
#endif

void platform_create_display(bool fullscreen)
{
    is_fullscreen = fullscreen;

    int flags = SDL_WINDOW_RESIZABLE;
    if (fullscreen)
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

#ifndef CHOWDREN_USE_D3D
    flags |= SDL_WINDOW_OPENGL;

#ifdef CHOWDREN_USE_GL
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#elif CHOWDREN_USE_GLES1
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#elif CHOWDREN_USE_GLES2
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

#ifdef CHOWDREN_USE_GL_DEBUG
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

#endif // CHOWDREN_USE_D3D

    int start_width = WINDOW_WIDTH;
    int start_height = WINDOW_HEIGHT;

#ifdef CHOWDREN_DEFAULT_SCALE
    start_width *= CHOWDREN_DEFAULT_SCALE;
    start_height *= CHOWDREN_DEFAULT_SCALE;
#endif

    global_window = SDL_CreateWindow(NAME,
                                     SDL_WINDOWPOS_CENTERED,
                                     SDL_WINDOWPOS_CENTERED,
                                     start_width, start_height,
                                     flags);
    global_window_id = SDL_GetWindowID(global_window);

    if (global_window == NULL) {
        std::cout << "Could not open window: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
        return;
    }

#ifdef __linux
    SDL_Surface * icon = SDL_LoadBMP("icon.bmp");
    if (icon == NULL) {
        std::cout << "Could not load icon.bmp" << std::endl;
    } else {
        SDL_SetWindowIcon(global_window, icon);
        SDL_FreeSurface(icon);
    }
#endif

#ifdef CHOWDREN_USE_D3D
    render_data.d3d = Direct3DCreate9(D3D_SDK_VERSION);
    SDL_SysWMinfo window_info;
    SDL_VERSION(&window_info.version);
    SDL_GetWindowWMInfo(global_window, &window_info);
    pparams.hDeviceWindow = window_info.info.win.window;
    pparams.BackBufferWidth = start_width;
    pparams.BackBufferHeight = start_height;
    pparams.BackBufferCount = 1;
    pparams.SwapEffect = D3DSWAPEFFECT_DISCARD;
    pparams.Windowed = TRUE;
    pparams.BackBufferFormat = D3DFMT_UNKNOWN;
    pparams.FullScreen_RefreshRateInHz = 0;
    pparams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    create_d3d_device();

    D3DCAPS9 caps;
    render_data.device->GetDeviceCaps(&caps);

    if ((caps.TextureCaps & D3DPTEXTURECAPS_POW2) != 0 &&
        (caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL) == 0)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "D3D error",
                                 "NPOT textures not supported", NULL);
        exit(EXIT_FAILURE);
    }

    render_data.device->GetRenderTarget(0, &render_data.default_target);
#else
    global_context = SDL_GL_CreateContext(global_window);
    if (global_context == NULL) {
        std::cout << "Could not create OpenGL context: " << SDL_GetError()
            << std::endl;
        exit(EXIT_FAILURE);
        return;
    }

    const GLubyte * renderer = glGetString(GL_RENDERER);
    const GLubyte * vendor = glGetString(GL_VENDOR);

    std::cout << "Renderer: " << renderer << " - " << vendor << " - "
        << std::endl;

#ifdef CHOWDREN_USE_GL
    // initialize OpenGL function pointers
    __glBlendEquationSeparateEXT =
        (PFNGLBLENDEQUATIONSEPARATEEXTPROC)
        SDL_GL_GetProcAddress("glBlendEquationSeparateEXT");
    __glBlendEquationEXT =
        (PFNGLBLENDEQUATIONEXTPROC)
        SDL_GL_GetProcAddress("glBlendEquationEXT");
    __glBlendFuncSeparateEXT =
        (PFNGLBLENDFUNCSEPARATEEXTPROC)
        SDL_GL_GetProcAddress("glBlendFuncSeparateEXT");

    __glActiveTextureARB =
        (PFNGLACTIVETEXTUREARBPROC)
        SDL_GL_GetProcAddress("glActiveTextureARB");
    __glClientActiveTextureARB =
        (PFNGLACTIVETEXTUREARBPROC)
        SDL_GL_GetProcAddress("glClientActiveTextureARB");

    __glGenFramebuffersEXT =
        (PFNGLGENFRAMEBUFFERSEXTPROC)
        SDL_GL_GetProcAddress("glGenFramebuffersEXT");
    __glDeleteFramebuffersEXT =
        (PFNGLDELETEFRAMEBUFFERSEXTPROC)
        SDL_GL_GetProcAddress("glDeleteFramebuffersEXT");
    __glFramebufferTexture2DEXT =
        (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)
        SDL_GL_GetProcAddress("glFramebufferTexture2DEXT");
    __glBindFramebufferEXT =
        (PFNGLBINDFRAMEBUFFEREXTPROC)
        SDL_GL_GetProcAddress("glBindFramebufferEXT");

    // shaders
    __glUniform1iARB =
        (PFNGLUNIFORM1IARBPROC)
        SDL_GL_GetProcAddress("glUniform1iARB");
    __glUseProgramObjectARB =
        (PFNGLUSEPROGRAMOBJECTARBPROC)
        SDL_GL_GetProcAddress("glUseProgramObjectARB");
    __glDetachObjectARB =
        (PFNGLDETACHOBJECTARBPROC)
        SDL_GL_GetProcAddress("glDetachObjectARB");
    __glGetInfoLogARB =
        (PFNGLGETINFOLOGARBPROC)
        SDL_GL_GetProcAddress("glGetInfoLogARB");
    __glGetObjectParameterivARB =
        (PFNGLGETOBJECTPARAMETERIVARBPROC)
        SDL_GL_GetProcAddress("glGetObjectParameterivARB");
    __glLinkProgramARB =
        (PFNGLLINKPROGRAMARBPROC)
        SDL_GL_GetProcAddress("glLinkProgramARB");
    __glCreateProgramObjectARB =
        (PFNGLCREATEPROGRAMOBJECTARBPROC)
        SDL_GL_GetProcAddress("glCreateProgramObjectARB");
    __glAttachObjectARB =
        (PFNGLATTACHOBJECTARBPROC)
        SDL_GL_GetProcAddress("glAttachObjectARB");
    __glCompileShaderARB =
        (PFNGLCOMPILESHADERARBPROC)
        SDL_GL_GetProcAddress("glCompileShaderARB");
    __glShaderSourceARB =
        (PFNGLSHADERSOURCEARBPROC)
        SDL_GL_GetProcAddress("glShaderSourceARB");
    __glCreateShaderObjectARB =
        (PFNGLCREATESHADEROBJECTARBPROC)
        SDL_GL_GetProcAddress("glCreateShaderObjectARB");
    __glUniform2fARB =
        (PFNGLUNIFORM2FARBPROC)
        SDL_GL_GetProcAddress("glUniform2fARB");
    __glUniform1fARB =
        (PFNGLUNIFORM1FARBPROC)
        SDL_GL_GetProcAddress("glUniform1fARB");
    __glUniform4fARB =
        (PFNGLUNIFORM4FARBPROC)
        SDL_GL_GetProcAddress("glUniform4fARB");
    __glGetUniformLocationARB =
        (PFNGLGETUNIFORMLOCATIONARBPROC)
        SDL_GL_GetProcAddress("glGetUniformLocationARB");
#endif

#ifdef CHOWDREN_USE_GL_DEBUG
#define GL_DEBUG_OUTPUT 0x92E0
	std::cout << "OpenGL debug enabled" << std::endl;

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
    if (SDL_GL_ExtensionSupported("GL_ARB_debug_output")) {
        std::cout << "Using ARB debug" << std::endl;
        PFNGLDEBUGMESSAGECALLBACKARBPROC glDebugMessageCallback =
            (PFNGLDEBUGMESSAGECALLBACKARBPROC)
            SDL_GL_GetProcAddress("glDebugMessageCallback");

        PFNGLDEBUGMESSAGECONTROLARBPROC glDebugMessageControl =
            (PFNGLDEBUGMESSAGECONTROLARBPROC)
            SDL_GL_GetProcAddress("glDebugMessageControl");

    	glDebugMessageCallback((GLDEBUGPROCARB)on_debug_message, NULL);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0,
                              NULL, GL_TRUE);
    } else if (SDL_GL_ExtensionSupported("GL_AMD_debug_output")) {
        std::cout << "Using AMD debug" << std::endl;
        PFNGLDEBUGMESSAGECALLBACKAMDPROC glDebugMessageCallbackAMD =
            (PFNGLDEBUGMESSAGECALLBACKAMDPROC)
            SDL_GL_GetProcAddress("glDebugMessageCallbackAMD");
        glDebugMessageCallbackAMD((GLDEBUGPROCAMD)on_debug_message_amd, NULL);
    }
#endif

    // check extensions
    if (!check_opengl_extensions()) {
        std::cout << "Not all OpenGL extensions supported. Quitting..."
            << std::endl;
        exit(EXIT_FAILURE);
        return;
    }

#endif // CHOWDREN_USE_D3D

    // if the cursor was hidden before the window was created, hide it now
    if (hide_cursor)
        platform_hide_mouse();

    screen_fbo.init(WINDOW_WIDTH, WINDOW_HEIGHT);
}

void platform_set_vsync(bool value)
{
    if (global_window == NULL)
        return;

    int vsync;
    if (value)
        vsync = 1;
    else
        vsync = 0;

    if (vsync == vsync_value)
        return;
    vsync_value = vsync;

#ifdef CHOWDREN_USE_D3D
    if (vsync == 1)
        pparams.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    else
        pparams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    d3d_do_reset = true;
#else
    int ret = SDL_GL_SetSwapInterval(vsync);
    if (ret == 0)
        return;

    std::cout << "Set vsync failed: " << SDL_GetError() << std::endl;
#endif
}

bool platform_get_vsync()
{
#ifdef CHOWDREN_USE_D3D
    return pparams.PresentationInterval == D3DPRESENT_INTERVAL_ONE;
#else
    int vsync = SDL_GL_GetSwapInterval();
    if (vsync != -1)
        return vsync == 1;
    switch (vsync_value) {
        case 1:
        case -1:
            return true;
        default:
            return false;
    }
#endif
}

void platform_set_fullscreen(bool value)
{
#ifdef CHOWDREN_NO_FULLSCREEN
    return;
#endif
    if (value == is_fullscreen)
        return;
    is_fullscreen = value;
    int flags;
    if (value)
        flags = SDL_WINDOW_FULLSCREEN_DESKTOP;
    else
        flags = 0;

#ifdef CHOWDREN_USE_D3D
    d3d_do_reset = true;
#else
    SDL_SetWindowFullscreen(global_window, flags);
    if (value)
        return;
    SDL_SetWindowPosition(global_window,
                          SDL_WINDOWPOS_CENTERED,
                          SDL_WINDOWPOS_CENTERED);
#endif

}

void platform_begin_draw()
{
#ifdef CHOWDREN_USE_D3D
    if (d3d_do_reset) {
        d3d_reset(false);
        d3d_do_reset = false;
    }

    render_data.device->BeginScene();
#else
    set_gl_state();
#endif

#ifdef CHOWDREN_SUBAPP_FRAMES
    static int old_width = WINDOW_START_WIDTH;
    static int old_height = WINDOW_START_HEIGHT;
    int w, h;
    platform_get_size(&w, &h);
    if (w != old_width || h != old_height) {
        old_width = w;
        old_height = h;
        screen_fbo.destroy();
        screen_fbo.init(w, h);
    }
#endif
    screen_fbo.bind();
}

void platform_swap_buffers()
{
#ifdef CHOWDREN_USE_GL_DEBUG
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
        std::cout << "OpenGL error: " << err << std::endl;
#endif

    int window_width, window_height;
    platform_get_size(&window_width, &window_height);
    bool resize = window_width != WINDOW_WIDTH ||
                  window_height != WINDOW_HEIGHT;

#ifdef CHOWDREN_FORCE_FILL
    int use_scale_type = EXACT_FIT;
#else
    int use_scale_type = scale_type;
    if (!is_fullscreen)
        use_scale_type = BEST_FIT;
#endif

    if (resize && use_scale_type == EXACT_FIT) {
        draw_x_size = window_width;
        draw_y_size = window_height;
    } else if (resize) {
        // aspect-aware resize
        float aspect_width = window_width / float(WINDOW_WIDTH);
        float aspect_height = window_height / float(WINDOW_HEIGHT);

        float aspect = std::min(aspect_width, aspect_height);
        if (use_scale_type == BEST_FIT_INT)
            aspect = floor(aspect);

        draw_x_size = aspect * WINDOW_WIDTH;
        draw_y_size = aspect * WINDOW_HEIGHT;
    } else {
        draw_x_size = WINDOW_WIDTH;
        draw_y_size = WINDOW_HEIGHT;
    }

    draw_x_off = (window_width - draw_x_size) / 2;
    draw_y_off = (window_height - draw_y_size) / 2;

    screen_fbo.unbind();

    // resize the window contents if necessary (fullscreen mode)
    Render::set_view(0, 0, window_width, window_height);
    Render::set_offset(0, 0);
    if (draw_x_size != window_width || draw_y_size != window_height) {
        Render::clear(0, 0, 0, 255);
    }

    int x2 = draw_x_off + draw_x_size;
    int y2 = draw_y_off + draw_y_size;

#ifdef CHOWDREN_SPECIAL_POINT_FILTER
    float x_scale = draw_x_size / float(WINDOW_WIDTH);
    float y_scale = draw_y_size / float(WINDOW_WIDTH);
    float use_effect = draw_x_size >= WINDOW_WIDTH &&
                       draw_y_size >= WINDOW_HEIGHT &&
                       (draw_x_size % WINDOW_WIDTH != 0 ||
                        draw_y_size % WINDOW_HEIGHT != 0) &&
                       !disable_resize_shader;
    if (use_effect) {        
        Render::set_effect(Render::PIXELSCALE);
        set_scale_uniform(WINDOW_WIDTH, WINDOW_HEIGHT,
                          x_scale, y_scale);
    }
#endif

    Render::disable_blend();
#ifdef CHOWDREN_USE_D3D
    Render::draw_tex(draw_x_off, draw_y_off, x2, y2, Color(),
                     screen_fbo.get_tex());
#else
	Render::draw_tex(draw_x_off, y2, x2, draw_y_off, Color(),
                     screen_fbo.get_tex());
#endif
    Render::enable_blend();

#ifdef CHOWDREN_SPECIAL_POINT_FILTER
    if (use_effect)
        Render::disable_effect();
#endif

#ifdef CHOWDREN_USE_D3D
    render_data.device->EndScene();
    if (FAILED(render_data.device->Present(NULL, NULL, NULL, NULL))) {
        std::cout << "Failed present: " << is_fullscreen << std::endl;
        d3d_reset(true);
    }
#else
#ifdef CHOWDREN_USE_GL
    // we need to unbind buffers and programs due to a bug in Steam overlay
    // and Geforce drivers.
    //
    // http://steamcommunity.com/groups/steamworks/discussions/12/
    // 666828127036283052/
    //
    // http://steamcommunity.com/groups/steamworks/discussions/12/
    // 618463446164757945/
    //
    glUseProgramObject(0);
    glVertexPointer(4, GL_FLOAT, 0, 0);
    glColorPointer(4, GL_FLOAT, 0, 0);
    glTexCoordPointer(4, GL_FLOAT, 0, 0);
#endif
    SDL_GL_SwapWindow(global_window);
#endif
}

void platform_get_size(int * width, int * height)
{
#ifdef CHOWDREN_USE_D3D
    *width = pparams.BackBufferWidth;
    *height = pparams.BackBufferHeight;
#else
    SDL_GL_GetDrawableSize(global_window, width, height);
#endif
}

void platform_get_screen_size(int * width, int * height)
{
    int display_index;
    display_index = SDL_GetWindowDisplayIndex(global_window);
    SDL_Rect bounds;
    SDL_GetDisplayBounds(display_index, &bounds);
    *width = bounds.w;
    *height = bounds.h;
}

void platform_set_display_scale(int scale)
{
    int nw = WINDOW_WIDTH * scale;
    int nh = WINDOW_HEIGHT * scale;
    int w, h;
    SDL_GetWindowSize(global_window, &w, &h);
    if (w == nw && h == nh)
        return;
    std::cout << "Set display scale: "
        << w << " " << nw << " "
        << h << " " << nh << std::endl;
    SDL_SetWindowSize(global_window, nw, nh);
    SDL_SetWindowPosition(global_window,
                          SDL_WINDOWPOS_CENTERED,
                          SDL_WINDOWPOS_CENTERED);
}

void platform_set_scale_type(int type)
{
    scale_type = type;
}

bool platform_has_focus()
{
    int f = SDL_GetWindowFlags(global_window);
    if ((f & SDL_WINDOW_SHOWN) == 0)
        return false;
    if ((f & SDL_WINDOW_INPUT_FOCUS) == 0)
        return false;
    return true;
}

void platform_set_focus(bool value)
{
    if (value)
        SDL_RestoreWindow(global_window);
    else
        SDL_MinimizeWindow(global_window);
}

void platform_show_mouse()
{
    hide_cursor = false;
    SDL_ShowCursor(1);
}

void platform_hide_mouse()
{
    hide_cursor = true;
    SDL_ShowCursor(0);
}

#if defined(CHOWDREN_USE_STEAM_LANGUAGE) && defined(CHOWDREN_ENABLE_STEAM)

const std::string & get_steam_language();
const std::string & platform_get_language()
{
    return get_steam_language();
}

#else

const std::string & platform_get_language()
{
    static std::string language("English");
    return language;
}

#endif

// filesystem stuff

#ifndef CHOWDREN_IS_ANDROID

#include <sys/stat.h>

void platform_walk_folder(const std::string & in_path,
                          FolderCallback & callback)
{
    if (in_path.empty())
        return;
    std::string path = convert_path(in_path);
#ifdef _WIN32
    HANDLE hFind = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA ffd;

    std::string spec;
    char c = path[path.size()-1];
    if (c == '\\')
        spec = path + "*";
    else
        spec = path + "\\*";

    hFind = FindFirstFileA(spec.c_str(), &ffd);
    if (hFind == INVALID_HANDLE_VALUE)
        return;

    FilesystemItem item;
    do {
        if (ffd.cFileName[0] == '.')
            continue;
        item.name = ffd.cFileName;
        item.flags = 0;
        if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            item.flags |= FilesystemItem::FILE;
        callback.on_item(item);
    } while (FindNextFileA(hFind, &ffd) != 0);

    FindClose(hFind);
#else
    class dirent *ent;
    class stat st;

    DIR * dir = opendir(path.c_str());
    if (dir == NULL)
        return;
    FilesystemItem item;
    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_name[0] == '.')
            continue;
        item.name = ent->d_name;
        item.flags = 0;
        if (ent->d_type != DT_DIR)
            item.flags |= FilesystemItem::FILE;
        callback.on_item(item);
    }
    closedir(dir);
#endif
}

size_t platform_get_file_size(const char * filename)
{
#ifndef _WIN32
    struct stat path_stat;
    if (stat(filename, &path_stat) != 0)
        return 0;
    if (!S_ISREG(path_stat.st_mode))
        return 0;
    return path_stat.st_size;
#else
    WIN32_FILE_ATTRIBUTE_DATA fad;

    if (GetFileAttributesExA(filename, GetFileExInfoStandard, &fad) == 0)
        return 0;

    if ((fad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
        return 0;

    size_t size = (uint64_t(fad.nFileSizeHigh) << (sizeof(fad.nFileSizeLow)*8))
                  + fad.nFileSizeLow;
    return size;
#endif
}

enum FileType
{
    NoneFile,
    RegularFile,
    Directory
};

static FileType get_file_type(const char * filename)
{
#ifndef _WIN32
    struct stat path_stat;
    if (stat(filename, &path_stat) != 0)
        return NoneFile;
    if (S_ISDIR(path_stat.st_mode))
        return Directory;
    if (S_ISREG(path_stat.st_mode))
        return RegularFile;
    return NoneFile;
#else

    DWORD attr(GetFileAttributesA(filename));
    if (attr == 0xFFFFFFFF)
        return NoneFile;
    if (attr & FILE_ATTRIBUTE_DIRECTORY)
        return Directory;
    else
        return RegularFile;
#endif
}

bool platform_path_exists(const std::string & value)
{
    return get_file_type(value.c_str()) != NoneFile;
}

bool platform_is_directory(const std::string & value)
{
    return get_file_type(value.c_str()) == Directory;
}

bool platform_is_file(const std::string & value)
{
    return get_file_type(value.c_str()) == RegularFile;
}

static bool create_dirs(const std::string & directory)
{
    if (directory.empty())
        return true;

    if (get_file_type(directory.c_str()) != NoneFile)
        return true;

    size_t slash = directory.find_last_of(PATH_SEP);
    if (slash != std::string::npos) {
        if (!create_dirs(directory.substr(0, slash)))
            return false;
    }

#ifdef _WIN32
    BOOL result = CreateDirectoryA(directory.c_str(), NULL);
    if (CreateDirectoryA(directory.c_str(), NULL) == FALSE)
        return false;
#else
    if (mkdir(directory.c_str(), S_IRWXU|S_IRWXG|S_IRWXO) != 0)
        return false;
#endif
    return true;
}

void platform_create_directories(const std::string & value)
{
    create_dirs(value);
}

const std::string & platform_get_appdata_dir()
{
    static bool initialized = false;
    static std::string dir;
    if (!initialized) {
#ifdef _WIN32
        TCHAR path[MAX_PATH];
        SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path);
        dir = path;
#elif __APPLE__
        FSRef ref;
        OSType folderType = kApplicationSupportFolderType;
        char path[PATH_MAX];
        FSFindFolder(kUserDomain, folderType, kCreateFolder, &ref);
        FSRefMakePath(&ref, (UInt8*)&path, PATH_MAX);
        dir = path;
#elif __linux
        struct passwd *pw = getpwuid(getuid());
        dir = pw->pw_dir;
#endif
    }
    return dir;
}

#endif // CHOWDREN_IS_ANDROID

// joystick

static SDL_HapticEffect rumble_effect;

class JoystickData
{
public:
    SDL_Joystick * joy;
    SDL_GameController * controller;
    int instance;
    SDL_Haptic * haptics;
    bool has_effect, has_rumble;
    int axis_count;
    int last_press;
    int button_count;
    int hat_count;

    JoystickData()
    : has_effect(false), has_rumble(false), last_press(0), controller(NULL),
      haptics(NULL)
    {
    }

    void init(SDL_GameController * c, SDL_Joystick * j, int i)
    {
        controller = c;
        joy = j;
        instance = i;
        if (c == NULL) {
            button_count = SDL_JoystickNumButtons(j);
            hat_count = SDL_JoystickNumHats(j);
        }
        init_rumble();
    }

    void close()
    {
        if (controller != NULL)
            SDL_GameControllerClose(controller);
        if (haptics != NULL)
            SDL_HapticClose(haptics);
        controller = NULL;
        haptics = NULL;
    }

    void init_rumble()
    {
        if (SDL_JoystickIsHaptic(joy) != 1)
            return;
        haptics = SDL_HapticOpenFromJoystick(joy);
        if (haptics == NULL)
            return;
        int efx = SDL_HapticEffectSupported(haptics, &rumble_effect);
        if (efx == 1) {
            rumble_effect.leftright.large_magnitude = 0;
            rumble_effect.leftright.small_magnitude = 0;
            SDL_HapticNewEffect(haptics, &rumble_effect);
            has_effect = true;
        } else if (SDL_HapticRumbleSupported(haptics) == 1) {
            SDL_HapticRumbleInit(haptics);
            has_rumble = true;
        }
    }

    float get_axis(int n)
    {
        if (controller == NULL)
            return SDL_JoystickGetAxis(joy, n) / float(0x7FFF);
        if (n <= SDL_CONTROLLER_AXIS_INVALID || n >= SDL_CONTROLLER_AXIS_MAX)
            return 0.0f;
        return SDL_GameControllerGetAxis(controller,
                                         (SDL_GameControllerAxis)n)
               / float(0x7FFF);
    }

    bool get_button(int b)
    {
        if (controller == NULL)
            return get_button((SDL_GameControllerButton)b);
        if (b <= SDL_CONTROLLER_BUTTON_INVALID ||
            b >= SDL_CONTROLLER_BUTTON_MAX)
            return false;
        return get_button((SDL_GameControllerButton)b);
    }

    bool get_joy_button(int n)
    {
        return SDL_JoystickGetButton(joy, n) == 1;
    }

    int get_joy_hat(int n)
    {
        if (n >= hat_count)
            return 0;
        return SDL_JoystickGetHat(joy, n);
    }

    bool get_button(SDL_GameControllerButton b)
    {
        if (controller == NULL) {
            int bb;
            switch (b) {
                case SDL_CONTROLLER_BUTTON_DPAD_UP:
                    return (get_joy_hat(0) & SDL_HAT_UP) != 0;
                case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                    return (get_joy_hat(0) & SDL_HAT_DOWN) != 0;
                case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                    return (get_joy_hat(0) & SDL_HAT_LEFT) != 0;
                case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                    return (get_joy_hat(0) & SDL_HAT_RIGHT) != 0;
                default:
                    bb = (int)b;
            }
            if (bb >= SDL_CONTROLLER_BUTTON_DPAD_UP)
                bb -= 4;
            if (bb >= button_count)
                return false;
            return SDL_JoystickGetButton(joy, bb) == 1;
        }
        return SDL_GameControllerGetButton(controller, b) == 1;
    }

    void vibrate(float left, float right, int ms)
    {
        if (!has_rumble && !has_effect)
            return;

        if (has_effect) {
            int lm = (int)(65535.0f * left);
            int sm = (int)(65535.0f * right);
            rumble_effect.leftright.large_magnitude = lm;
            rumble_effect.leftright.small_magnitude = sm;
            rumble_effect.leftright.length = ms;
            SDL_HapticUpdateEffect(haptics, 0, &rumble_effect);
            SDL_HapticRunEffect(haptics, 0, 1);
        } else {
            float strength;
            if (left >= right) {
                strength = left;
            } else {
                strength = right;
            }
            SDL_HapticRumblePlay(haptics, strength, ms);
        }
    }
};

static int selected_joy_index = 0;
static JoystickData * selected_joy = NULL;
static vector<JoystickData> joysticks;

#define CHOWDREN_SINGLE_JOYSTICK

JoystickData & get_joy(int n)
{
#ifdef CHOWDREN_SINGLE_JOYSTICK
    return *selected_joy;
#else
    return joysticks[n-1];
#endif
}

JoystickData * get_joy_instance(int instance)
{
    vector<JoystickData>::iterator it;
    for (it = joysticks.begin(); it != joysticks.end(); ++it) {
        JoystickData & j = *it;
        if (j.instance != instance)
            continue;
        return &j;
    }
    return NULL;
}

void add_joystick(int device)
{
    SDL_GameController * c = NULL;
    if (SDL_IsGameController(device))
        c = SDL_GameControllerOpen(device);

    SDL_Joystick * joy = NULL;
    if (c == NULL)
        joy = SDL_JoystickOpen(device);
    else
        joy = SDL_GameControllerGetJoystick(c);
    vector<JoystickData>::iterator it;
    for (it = joysticks.begin(); it != joysticks.end(); ++it) {
        JoystickData & j = *it;
        if (j.joy == joy) {
            if (c != NULL)
                SDL_GameControllerClose(c);
            else
                SDL_JoystickClose(joy);
            return;
        }
    }
    int instance = SDL_JoystickInstanceID(joy);
    int index = joysticks.size();
    joysticks.resize(index+1);
    joysticks[index].init(c, joy, instance);
    selected_joy_index = index;
    selected_joy = &joysticks[selected_joy_index];
}

void remove_joystick(int instance)
{
    if (selected_joy != NULL && selected_joy->instance == instance) {
        selected_joy = NULL;
        selected_joy_index = 0;
    }

    vector<JoystickData>::iterator it;
    for (it = joysticks.begin(); it != joysticks.end(); ++it) {
        JoystickData & j = *it;
        if (j.instance != instance)
            continue;
        j.close();
        joysticks.erase(it);
        return;
    }
}

void init_joystick()
{
    SDL_SetHint(SDL_HINT_ACCELEROMETER_AS_JOYSTICK, "0");
    SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");
    SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC);
    SDL_GameControllerAddMappingsFromFile("gamecontrollerdb-override.txt");

    rumble_effect.type = SDL_HAPTIC_LEFTRIGHT;
    rumble_effect.leftright.length = 0;

    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        add_joystick(i);
    }
}

void update_joystick()
{
}

void on_joystick_button(int instance, int button, bool state)
{
    JoystickData * joy = NULL;
    unsigned int i;
    for (i = 0; i < joysticks.size(); ++i) {
        JoystickData & j = joysticks[i];
        if (j.instance != instance)
            continue;
        joy = &j;
        break;
    }

    selected_joy_index = int(i);
    selected_joy = joy;
    if (!state)
        return;
    if (joy->controller != NULL)
        return;
    if (button >= SDL_CONTROLLER_BUTTON_DPAD_UP)
        button += 4;
    joy->last_press = button;
}

void on_controller_button(int instance, int button, bool state)
{
    JoystickData * joy = get_joy_instance(instance);
    joy->last_press = button;
}

int get_joystick_last_press(int n)
{
    if (!is_joystick_attached(n))
        return CHOWDREN_BUTTON_INVALID;
    return unremap_button(get_joy(n).last_press + 1);
}

extern std::string empty_string;

const std::string & get_joystick_name(int n)
{
    if (!is_joystick_attached(n))
        return empty_string;
    static std::string ret;
    JoystickData & joy = get_joy(n);
#ifdef CHOWDREN_FORCE_X360
    ret = "X360 Controller";
#else
    if (joy.controller == NULL)
        ret = SDL_JoystickName(joy.joy);
    else
        ret = SDL_GameControllerName(joy.controller);
#endif
    return ret;
}

const std::string & get_joystick_guid(int n)
{
    if (!is_joystick_attached(n))
        return empty_string;
    static std::string ret;
    ret.resize(64);
    JoystickData & joy = get_joy(n);
    SDL_JoystickGUID guid = SDL_JoystickGetGUID(joy.joy);
    SDL_JoystickGetGUIDString(guid, &ret[0], 64);
    ret.resize(strlen(&ret[0]));
    return ret;
}

bool is_joystick_attached(int n)
{
    n--;
#ifdef CHOWDREN_SINGLE_JOYSTICK
    if (n != 0)
        return false;
    return selected_joy != NULL;
#else
    return n >= 0 && n < int(joysticks.size());
#endif
}

bool is_joystick_pressed(int n, int button)
{
    if (!is_joystick_attached(n))
        return false;
    button = remap_button(button);
    button--;
    return get_joy(n).get_button(button);
}

bool any_joystick_pressed(int n)
{
    if (!is_joystick_attached(n))
        return false;
    JoystickData & joy = get_joy(n);
    if (joy.controller == NULL) {
        for (int i = 0; i < joy.button_count; i++) {
            if (joy.get_joy_button(i)) {
                return true;
            }
        }
        return false;
    }
    for (unsigned int i = 0; i < SDL_CONTROLLER_BUTTON_DPAD_UP; i++) {
        if (joy.get_button(i)) {
            return true;
        }
    }
    return false;
}

bool is_joystick_released(int n, int button)
{
    if (!is_joystick_attached(n))
        return true;
    button = remap_button(button);
    button--;
    return !get_joy(n).get_button(button);
}

void joystick_vibrate(int n, int l, int r, int ms)
{
    if (!is_joystick_attached(n))
        return;
    get_joy(n).vibrate(l / 100.0f, r / 100.0f, ms);
}

float get_joystick_axis_raw(int n, int axis)
{
    if (!is_joystick_attached(n))
        return 0.0f;
    axis--;
    return get_joy(n).get_axis(axis);
}

// url open

#ifdef _WIN32

void open_url(const std::string & name)
{
    ShellExecute(NULL, "open", name.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

#elif __APPLE__

void open_url(const std::string & name)
{
    CFStringRef str = CFStringCreateWithCString(0, name.c_str(), 0);
    CFURLRef ref = CFURLCreateWithString(0, str, 0);
    LSOpenCFURLRef(ref, 0);
    CFRelease(ref);
    CFRelease(str);
}

#elif __linux

#ifdef CHOWDREN_ENABLE_STEAM
// http://steamcommunity.com/groups/steamworks/discussions/13/
// 618463738391189582/
#define ESCAPE_STEAM_RUNTIME ("STEAM_RUNTIME=0 "\
                              "LD_LIBRARY_PATH=\"$SYSTEM_LD_LIBRARY_PATH\" "\
                              "PATH=\"$SYSTEM_PATH\" ")

static char transform_temp[1024];
inline const char * transform_command(const char * data)
{
    if (getenv("STEAM_RUNTIME") == NULL)
        return data;
    transform_temp[0] = '\0';
    strcat(transform_temp, ESCAPE_STEAM_RUNTIME);
    strcat(transform_temp, data);
    return &transform_temp[0];
}
#else

inline const char * transform_command(const char * data)
{
    return data;
}

#endif


extern "C" int chow_system(const char * cmd)
{
    return system(transform_command(cmd));
}

extern "C" FILE * chow_popen(const char * cmd, const char * mode)
{
    return popen(transform_command(cmd), mode);
}

void open_url(const std::string & name)
{
    std::string cmd;
    cmd += "xdg-open '";
    cmd += name;
    cmd += "' &";
    chow_system(cmd.c_str());
}

#endif

// file

#ifdef CHOWDREN_AUTO_STEAMCLOUD
#include "steam/steam_api.h"
#include "steam/steamtypes.h"
#include <sstream>
#include "../path.h"
#endif

bool platform_remove_file(const std::string & file)
{
#ifdef CHOWDREN_AUTO_STEAMCLOUD
    std::string base = get_path_filename(file);
    const char * base_c = base.c_str();
    if (SteamRemoteStorage()->FileExists(base_c)) {
        return SteamRemoteStorage()->FileDelete(base_c);
    }
#endif
    return remove(convert_path(file).c_str()) == 0;
}

static bool remove_directory_recurse(const std::string & path);

static bool remove_directory(const char * filename)
{
#ifdef _WIN32
    return RemoveDirectory(filename) != 0;
#else
    return rmdir(filename) == 0;
#endif
}

struct RemoveDirectoryCallback : FolderCallback
{
    const std::string & path;

	RemoveDirectoryCallback(const std::string & path)
    : path(path)
    {
    }

    void on_item(FilesystemItem & item)
    {
        if (item.is_file()) {
            remove(join_path(path, item.name).c_str());
        } else if (item.is_folder()) {
            std::string new_path = join_path(path, item.name);
            remove_directory_recurse(new_path);
        }
    }
};

static bool remove_directory_recurse(const std::string & dir)
{
    if (remove_directory(dir.c_str()))
        return true;
    RemoveDirectoryCallback callback(dir);
    platform_walk_folder(dir, callback);
	if (remove_directory(dir.c_str()))
        return true;
    std::cout << "Could not remove directory: " << dir << std::endl;
    return false;
}

bool platform_remove_directory(const std::string & dir)
{
    std::string path = convert_path(dir);
    return remove_directory_recurse(path);
}

#include "fileio.cpp"

#if defined(CHOWDREN_IS_ANDROID) || defined(CHOWDREN_IS_EMSCRIPTEN)
#include "sdlfile.cpp"
#else
#include "stdiofile.cpp"
#endif

// path

std::string convert_path(const std::string & v)
{
    std::string value = v;
    if (value.compare(0, 3, "./\\") == 0)
        value = std::string("./", 2) + value.substr(3);

#ifndef _WIN32
    std::replace(value.begin(), value.end(), '\\', '/');
#else
    std::replace(value.begin(), value.end(), '/', '\\');
#endif

#ifdef CHOWDREN_IS_ANDROID
    if (value.compare(0, 2, "./") == 0)
        value = value.substr(2);
#endif
    return value;
}

// dialog

#ifdef CHOWDREN_IS_DESKTOP

#include "tinyfiledialogs.h"

#ifdef _WIN32
#include <direct.h>
#define chdir _chdir
#define getcwd _getcwd
#define PATH_MAX 4096
#else
#include <limits.h>
#endif

#define SAVE_CWD() char temp[PATH_MAX];\
                   if (!getcwd(temp, PATH_MAX)) temp[0] = '\0'
#define RESTORE_CWD() (void)chdir(temp)

bool platform_file_open_dialog(const std::string & title,
                               const std::string & filter,
                               const std::string & in_def,
                               bool multiple,
                               vector<std::string> & out)
{
    SAVE_CWD();
    std::string def = convert_path(in_def);
    const char * ret = tinyfd_openFileDialog(title.c_str(),
                                             def.c_str(),
                                             0,
                                             NULL,
                                             NULL,
                                             multiple);
    if (ret != NULL)
        out.push_back(std::string(ret));
    RESTORE_CWD();
    return ret != NULL;
}

bool platform_file_save_dialog(const std::string & title,
                               const std::string & filter,
                               const std::string & in_def,
                               std::string & out)
{
    SAVE_CWD();
    std::string def = convert_path(in_def);
    const char * ret = tinyfd_saveFileDialog(title.c_str(),
                                             def.c_str(),
                                             0,
                                             NULL,
                                             NULL);
    if (ret != NULL)
        out = ret;
    RESTORE_CWD();
    return ret != NULL;
}

#include "dialoglocale.cpp"

bool platform_show_dialog(const std::string & title,
                          const std::string & message,
                          DialogType type)
{
#ifdef __APPLE__
    SDL_MessageBoxData data;
    data.flags = SDL_MESSAGEBOX_INFORMATION;
    data.window = global_window;
    data.title = title.c_str();
    data.message = message.c_str();
    int good;
    const char * first = NULL;
    const char * second = NULL;
    switch (type) {
        case DIALOG_OK:
            good = -1;
            first = get_locale_ok();
            second = NULL;
            break;
        case DIALOG_OKCANCEL:
            good = 1;
            first = get_locale_ok();
            second = get_locale_cancel();
            break;
        case DIALOG_YESNO:
            good = 1;
            first = get_locale_yes();
            second = get_locale_no();
            break;
    }
    SDL_MessageBoxButtonData buttons[2];
    data.buttons = buttons;
    data.numbuttons = 1;
    if (second == NULL) {
        buttons[0].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
        buttons[0].buttonid = 1;
        buttons[0].text = first;
    } else {
        buttons[0].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
        buttons[0].buttonid = 2;
        buttons[0].text = second;
        buttons[1].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
        buttons[1].buttonid = 1;
        buttons[1].text = first;
        data.numbuttons++;
    }
    data.colorScheme = NULL;
    int hit;
    if (SDL_ShowMessageBox(&data, &hit) != 0)
        return false;
    if (good == -1)
        return true;
    return hit == good;
#else
    const char * dialog_type = "ok";
    switch (type) {
        case DIALOG_OK:
            dialog_type = "ok";
            break;
        case DIALOG_OKCANCEL:
            dialog_type = "okcancel";
            break;
        case DIALOG_YESNO:
            dialog_type = "yesno";
            break;
    }
    int ret = tinyfd_messageBox(title.c_str(), message.c_str(), dialog_type,
                                "info", 1);
    return ret == 1;
#endif
}

#else

bool platform_file_open_dialog(const std::string & title,
                               const std::string & filter,
                               const std::string & in_def,
                               bool multiple,
                               vector<std::string> & out)
{
    return false;
}

bool platform_file_save_dialog(const std::string & title,
                               const std::string & filter,
                               const std::string & in_def,
                               std::string & out)
{
    return false;
}

bool platform_show_dialog(const std::string & title,
                          const std::string & message,
                          DialogType type)
{
    return false;
}

#endif

// debug

void platform_print_stats()
{

}



// dummies

void platform_prepare_frame_change()
{
}

void platform_set_remote_setting(const std::string & v)
{

}

void platform_set_remote_value(int v)
{

}

void platform_set_lightbar(int r, int g, int b, int ms, int type)
{
    // std::cout << "Set lightbar: " << r << " " << g << " " << b << " "
    //     << ms << " " << type << std::endl;
}

void platform_reset_lightbar()
{
    // std::cout << "Reset lightbar" << std::endl;
}

int platform_get_remote_value()
{
    return CHOWDREN_TV_TARGET;
}

void platform_set_border(bool v)
{

}

static std::string remote_string("TV");

const std::string & platform_get_remote_setting()
{
    return remote_string;
}

bool platform_has_error()
{
    return false;
}

void platform_debug(const std::string & value)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Debug",
                             value.c_str(), NULL);
}

void platform_exit()
{
#ifdef _WIN32
    timeEndPeriod(1);
#endif

    joysticks.clear();

    SDL_Quit();

#ifdef CHOWDREN_IS_ANDROID
    exit(0);
#endif
}

#ifndef CHOWDREN_IS_DESKTOP
void platform_unlock_achievement(const std::string & name)
{
}
#endif