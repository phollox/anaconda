#ifndef CHOWDREN_PLATFORM_H
#define CHOWDREN_PLATFORM_H

#include <string>
#include "fileio.h"

#ifdef CHOWDREN_IS_WIIU
#define IS_BIG_ENDIAN
#endif

void open_url(const std::string & name);
void platform_init();
void platform_exit();
void platform_poll_events();
bool platform_display_closed();
void platform_create_display(bool fullscreen);
void platform_get_size(int * width, int * height);
void platform_get_screen_size(int * width, int * height);
double platform_get_time();
unsigned int platform_get_global_time();
void platform_sleep(double v);
int translate_vk_to_key(int vk);
int translate_key_to_vk(int key);
int translate_string_to_key(const std::string & name);
bool platform_has_focus();
void platform_set_focus(bool v);
void platform_show_mouse();
void platform_get_mouse_pos(int * x, int * y);
void platform_hide_mouse();
void platform_begin_draw();
void platform_swap_buffers();
void platform_prepare_frame_change();
bool platform_remove_file(const std::string & path);
const std::string & platform_get_appdata_dir();
const std::string & platform_get_language();
void platform_set_vsync(bool value);
bool platform_get_vsync();
void platform_set_fullscreen(bool value);

// fs

size_t platform_get_file_size(const char * filename);
void platform_create_directories(const std::string & v);
bool platform_is_file(const std::string & path);
bool platform_is_directory(const std::string & path);
bool platform_path_exists(const std::string & path);

// debug
void platform_print_stats();
void platform_debug(const std::string & error);

// joystick
bool is_joystick_attached(int n);
bool is_joystick_pressed(int n, int button);
bool any_joystick_pressed(int n);
bool is_joystick_released(int n, int button);
bool compare_joystick_direction(int n, int test_dir);
bool is_joystick_direction_changed(int n);
void joystick_vibrate(int n, int l, int r, int d);
float get_joystick_axis(int n, int axis);
int get_joystick_last_press(int n);
const std::string & get_joystick_name(int n);

// desktop
void platform_set_display_scale(int scale);
void platform_set_scale_type(int type);

// path
std::string convert_path(const std::string & value);

// glc
void glc_init();
void glc_copy_color_buffer_rect(unsigned int tex, int x1, int y1, int x2,
                                     int y2);
void glc_scissor_world(int x, int y, int w, int h);
void glc_set_storage(bool vram);
bool glc_is_vram_full();

// demo

#ifdef CHOWDREN_IS_DEMO
bool platform_show_build_info();
bool platform_should_reset();
#endif

// wiiu

#define CHOWDREN_TV_TARGET 0
#define CHOWDREN_REMOTE_TARGET 1
#define CHOWDREN_HYBRID_TARGET 2
#define CHOWDREN_REMOTE_ONLY 3
void platform_clone_buffers();
void platform_set_display_target(int value);
void platform_set_remote_value(int value);
void platform_set_remote_setting(const std::string & v);
const std::string & platform_get_remote_setting();
int platform_get_remote_value();
unsigned int & platform_get_texture_pixel(unsigned int tex, int x, int y);
void platform_set_border(bool value);
bool platform_has_error();

#endif // CHOWDREN_PLATFORM_H
