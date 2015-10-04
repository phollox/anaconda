#ifndef CHOWDREN_SUBAPP_H
#define CHOWDREN_SUBAPP_H

#include "frameobject.h"
#include "events.h"

#ifdef CHOWDREN_USE_GWEN
#include "Gwen/Controls/WindowControl.h"
#endif

class SubApplication : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(SubApplication)

    static SubApplication * current;
    static int current_x, current_y;
    Frames subapp_frame;
    int frame_offset;
    bool done;
    bool starting;
    bool old_ignore_controls;

#ifdef CHOWDREN_USE_GWEN
    enum Flags {
        IS_POPUP = 1 << 0,
        IS_DOCKED = 1 << 1
    };

    bool gwen_close;
    int subapp_flags;
    int start_x, start_y;
    Gwen::Controls::WindowControl * window_control;
    int get_render_x();
    int get_render_y();
    void init_window();
    void init_frame();
#endif

    SubApplication(int x, int y, int id);
    ~SubApplication();
    void restart(int index);
    void update();
    void set_next_frame(int index);
    void set_frame(int index);
#ifdef CHOWDREN_SUBAPP_FRAMES
    void draw_subapp();
    static void draw_frames();
    static bool test_pos(Frame * frame);
#endif
};

#endif // CHOWDREN_SUBAPP_H
