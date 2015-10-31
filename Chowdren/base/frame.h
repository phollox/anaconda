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

// make sure frameobject.h is included first
#ifndef CHOWDREN_FRAMEOBJECT_H
#include "frameobject.h"
#endif

#ifndef CHOWDREN_FRAME_H
#define CHOWDREN_FRAME_H

#include "types.h"
#include <list>
#include "broadphase.h"
#include "frameobject.h"
#include "color.h"
#include "instancemap.h"
#include "bitarray.h"

#ifdef CHOWDREN_PASTE_CACHE
#include "fbo.h"
#endif

#ifdef CHOWDREN_USE_GWEN
#include "gui/gwen.h"
#endif

class Background;
class BackgroundItem;
class CollisionBase;

typedef vector<BackgroundItem*> BackgroundItems;
typedef boost::intrusive::member_hook<FrameObject, LayerPos,
                                      &FrameObject::layer_pos> LayerHook;
typedef boost::intrusive::list<FrameObject, LayerHook> LayerInstances;

class Layer
{
public:
    int off_x, off_y;
    int scroll_x, scroll_y;
    LayerInstances instances;
    FlatObjectList background_instances;
    bool visible;
    double coeff_x, coeff_y;
    Background * back;
    int index;
    bool scroll_active;
    int x, y;
    Broadphase broadphase;
    bool wrap_x, wrap_y;
    Color blend_color;
    int inactive_box[4];
    int kill_box[4];

#ifdef CHOWDREN_IS_3DS
    float depth;
#endif

    Layer();
    Layer(int index, double coeff_x, double coeff_y, bool visible,
          bool wrap_x, bool wrap_y);
    ~Layer();
    Layer(const Layer & layer);
    Layer & operator=(const Layer &);
    void init(int index, double coeff_x, double coeff_y, bool visible,
              bool wrap_x, bool wrap_y);
    void reset();
    void scroll(int off_x, int off_y, int dx, int dy);
    void set_position(int x, int y);
    void update_position();
    void add_background_object(FrameObject * instance);
    void remove_background_object(FrameObject * instance);
    void add_object(FrameObject * instance);
    void insert_object(FrameObject * instance, int index);
    void remove_object(FrameObject * instance);
    void reset_depth();
    int get_level(FrameObject * instance);
    void set_level(FrameObject * instance, int index);
    void destroy_backgrounds();
    void destroy_backgrounds(int x, int y, bool fine);
    CollisionBase * test_background_collision(CollisionBase * a);
    CollisionBase * test_background_collision(int x, int y);
    void paste(Image * img, int dest_x, int dest_y,
               int src_x, int src_y, int src_width, int src_height,
               int collision_type, int effect, const Color & color);
    void draw(int off_x, int off_y);
    void set_visible(bool value);
    void show();
    void hide();

#ifdef CHOWDREN_HAS_MRT
    int remote;
    void set_remote(int value);
#endif
};

#ifdef CHOWDREN_PASTE_PRECEDENCE
#include "image.h"
#include "collision.h"
#endif

class Background
{
public:
#ifdef CHOWDREN_PASTE_BROADPHASE
    Broadphase items;
    Broadphase col_items;
#else
    BackgroundItems items;
    BackgroundItems col_items;
#endif

#ifdef CHOWDREN_PASTE_CACHE
    struct BackgroundCache
    {
        bool dirty;
        bool clear;
        Framebuffer fbo;

        BackgroundCache()
        : dirty(false), clear(false)
        {
        }
    };

    BackgroundItems new_paste;
    BackgroundCache * cache;
    int cache_w, cache_h;
#endif

#ifdef CHOWDREN_PASTE_PRECEDENCE
    BitArray col;
    int col_w, col_h;
    Image col_img;
    SpriteCollision back_col;
#endif

    Background(Layer * layer);
    ~Background();
    void reset(bool clear_items = true);
    void destroy_at(int x, int y);
    void paste(Image * img, int dest_x, int dest_y,
               int src_x, int src_y, int src_width, int src_height,
               int collision_type, int effect, const Color & color);
    void draw(Layer * layer, int v[4]);
    CollisionBase * collide(CollisionBase * a);
    CollisionBase * overlaps(CollisionBase * a);
};


typedef void (*LoopCallback)(void*);

class DynamicLoop
{
public:
    LoopCallback callback;
    bool * running;
    int * index;

    DynamicLoop()
    {
    }

    void set(LoopCallback callback, bool * running, int * index)
    {
        this->callback = callback;
        this->running = running;
        this->index = index;
    }
};

typedef hash_map<std::string, DynamicLoop> DynamicLoops;

class GameManager;
class GlobalValues;
class GlobalStrings;
class Backdrop;

class FrameData
{
public:
    std::string name;

    FrameData();
    virtual void event_callback(Frame * frame, int id);
    virtual void init(Frame * frame);
    virtual void on_start(Frame * frame);
    virtual void on_end(Frame * frame);
    virtual void on_app_end(Frame * frame);
    virtual void handle_events(Frame * frame);
    virtual void handle_pre_events(Frame * frame);
};


class Frame
{
public:
    typedef void (Frame::*EventFunction)();

    int width, height;
    int virtual_width, virtual_height;
    int index;
    Color background_color;
    DynamicLoops * loops;
    FrameData * data;
    InstanceMap instances;
    FlatObjectList destroyed_instances;
    vector<Layer> layers;
    GlobalValues * global_values;
    GlobalStrings * global_strings;
    bool has_quit;
    int off_x, off_y, new_off_x, new_off_y;
    int last_key;
    int next_frame;
    unsigned int loop_count;
    double frame_time;
    int timer_base;
    float timer_mul;

#ifdef CHOWDREN_USE_BACKMAGIC
    FlatObjectList back_instances[MAX_BACK_ID];
    Backdrop * back_obj;
#endif

#ifdef CHOWDREN_USE_GWEN
    GwenData gwen;
    int old_mouse_x;
    int old_mouse_y;
#endif

#ifdef CHOWDREN_SUBAPP_FRAMES
    bool is_mouse_pressed_once_frame(int button);
    bool is_mouse_pressed_frame(int button);
    int display_width, display_height;
#else
    // inline methods? these are very unique names anyway
    #define is_mouse_pressed_frame is_mouse_pressed
    #define is_mouse_pressed_once_frame is_mouse_pressed_once
#endif

    FrameObject * col_instance_1;
    FrameObject * col_instance_2;

    Frame();
    void reset();
    bool update();
    virtual void update_objects();
    void pause();
    void restart();
    void draw(int remote);
    FrameObject * add_object(FrameObject * object, int layer_index);
    FrameObject * add_object(FrameObject * object, Layer * layer);
    void add_background_object(FrameObject * object, int layer_index);
    void set_object_layer(FrameObject * object, int new_layer);
    int get_loop_index(const std::string & name);
    void set_timer(double value);
    void set_lives(int lives);
    void set_score(int score);
    void set_display_center(int x = -1, int y = -1);
    void update_display_center();
    void set_background_color(const Color & color);
    void get_mouse_pos(int * x, int * y);
    int get_mouse_x();
    int get_mouse_y();
    bool mouse_in_zone(int x1, int y1, int x2, int y2);
    CollisionBase * test_background_collision(int x, int y);
    int get_background_mask(int x, int y);
    bool test_obstacle(int x, int y);
    bool test_ladder(int x, int y);
    bool compare_joystick_direction(int n, int test_dir);
    bool is_joystick_direction_changed(int n);
    void clean_instances();
    void set_vsync(bool value);
    int get_instance_count();
    void set_width(int width, bool adjust);
    void set_height(int height, bool adjust);

    void test_collisions(ObjectList & a, ObjectList & b,
                         int flag1, int flag2, EventFunction e);
    void test_collisions(QualifierList & a, ObjectList & b,
                         int flag1, int flag2, EventFunction e);
    void test_collisions(QualifierList & a, QualifierList & b,
                         int flag1, int flag2, EventFunction e);
    void test_collisions_save(ObjectList & a, ObjectList & b,
                              int flag1, int flag2, EventFunction e);
    void test_collisions_save(QualifierList & a, ObjectList & b,
                              int flag1, int flag2, EventFunction e);

    virtual void set_index(int index) = 0;
    virtual void load_static_images();

    // inline functions

    int frame_left()
    {
        return new_off_x;
    }

    int frame_right()
    {
        return new_off_x + WINDOW_WIDTH;
    }

    int frame_top()
    {
        return new_off_y;
    }

    int frame_bottom()
    {
        return new_off_y + WINDOW_HEIGHT;
    }

    void event_callback(int id)
    {
        data->event_callback(this, id);
    }

    void on_end()
    {
        data->on_end(this);
        reset();
    }

    void set_data(FrameData * data)
    {
        this->data = data;
    }
};

#endif // CHOWDREN_FRAME_H
