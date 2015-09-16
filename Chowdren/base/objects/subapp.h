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
    static int current_x;
    static int current_y;
    Frames subapp_frame;
    int frame_offset;
    bool done;
    bool starting;
    bool old_ignore_controls;

#ifdef CHOWDREN_USE_GWEN
    Gwen::Controls::WindowControl * window_control;
    void init_window();
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
    static bool test_pos(Frame * frame, int x, int y);
#endif
};

#endif // CHOWDREN_SUBAPP_H
