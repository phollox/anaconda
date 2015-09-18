#include "manager.h"
#include "subapp.h"

#ifdef CHOWDREN_SUBAPP_FRAMES
#include "collision.h"
#include "render.h"
#include "types.h"
#endif

static bool has_ignore_controls = false;
static bool ignore_controls = false;

#ifdef CHOWDREN_SUBAPP_FRAMES
static vector<SubApplication*> frames;
#endif

#define SET_APP() \
    Frame * old_frame = manager.frame;\
    manager.frame = &subapp_frame;\
    int old_x = current_x;\
    int old_y = current_y;\
    current_x += get_x();\
    current_y += get_y()

#define RESTORE_APP() \
    manager.frame = old_frame;\
    current_x = old_x;\
    current_y = old_y

SubApplication::SubApplication(int x, int y, int id)
: FrameObject(x, y, id)
{
    current = this;
    subapp_frame.instances.clear();
#ifdef CHOWDREN_SUBAPP_FRAMES
    frames.push_back(this);
    collision = new InstanceBox(this);
#endif

#ifdef CHOWDREN_USE_GWEN
    window_control = NULL;
#endif
}

SubApplication::~SubApplication()
{
#ifdef CHOWDREN_SUBAPP_FRAMES
    delete collision;
    frames.erase(std::remove(frames.begin(), frames.end(), this),
                 frames.end());
#endif

#ifdef CHOWDREN_USE_GWEN
    delete window_control;
#endif

    SET_APP();
    if (!done && !starting) {
        subapp_frame.data->on_app_end();
        subapp_frame.data->on_end();
    }
    RESTORE_APP();

	if (current == this)
		current = NULL;

    if (has_ignore_controls) {
        manager.ignore_controls = ignore_controls;
        has_ignore_controls = false; 
    }
}

void SubApplication::set_next_frame(int index)
{
    if (starting)
        return;
    subapp_frame.next_frame = index + frame_offset;
    std::cout << "next frame: " << index << std::endl;
}

void SubApplication::restart(int index)
{
    done = false;
    starting = true;
    subapp_frame.next_frame = index + frame_offset;
    std::cout << "restart: " << index << std::endl;
}

void SubApplication::update()
{
    if (done)
        return;
    starting = false;

    SET_APP();

#ifdef CHOWDREN_USE_GWEN
    subapp_frame.gwen.update();
    move_front();
#endif

    if (subapp_frame.next_frame != -1) {
        int next_frame = subapp_frame.next_frame;
        if (subapp_frame.index != -1)
            subapp_frame.on_end();
        set_frame(next_frame);
    }

#ifdef CHOWDREN_SUBAPP_FRAMES
    width = subapp_frame.width;
    height = subapp_frame.height;
#endif

    bool ret = subapp_frame.update();

    if (!ret)
        subapp_frame.on_end();

    RESTORE_APP();

    if (ret)
        return;
    done = true;
    manager.ignore_controls = old_ignore_controls;
    set_visible(false);
}

void SubApplication::set_frame(int index)
{
    done = false;
    subapp_frame.set_index(index);

    if (!has_ignore_controls) {
        ignore_controls = manager.ignore_controls;
        manager.ignore_controls = false;
        has_ignore_controls = true; 
    }
}

#ifdef CHOWDREN_USE_GWEN

void SubApplication::init_window()
{
    Gwen::Controls::Canvas * canvas = manager.frame->gwen.canvas;
    window_control = new Gwen::Controls::WindowControl(canvas);
    window_control->SetPos(x-6, y-28);
    window_control->SetSize(width+12, height+35);
    window_control->SetClosable(false);
    window_control->DisableResizing();
}
#endif

#ifdef CHOWDREN_SUBAPP_FRAMES
void SubApplication::draw_subapp()
{
    if (starting || done || !(flags & VISIBLE))
        return;
    if (window_control) {
        frame->gwen.render(window_control);
        Gwen::Point p = window_control->GetPos();
        set_position(p.x + 6, p.y + 28);
        window_control->SetSize(width+12, height+35);
    }
    SET_APP();
    Render::enable_scissor(current_x, current_y, width, height);
    subapp_frame.draw(0);
    RESTORE_APP();
    Render::disable_scissor();
}

void SubApplication::draw_frames()
{
    vector<SubApplication*>::iterator it;
    for (it = frames.begin(); it != frames.end(); ++it) {
        Render::set_offset(0, 0);
        (*it)->draw_subapp();
    }
}

bool SubApplication::test_pos(Frame * frame, int x, int y)
{
    PointCollision p(x, y);
    vector<SubApplication*>::reverse_iterator it;
    for (it = frames.rbegin(); it != frames.rend(); ++it) {
        SubApplication * subapp = *it;
        if (&subapp->subapp_frame == frame)
            return false;
        if (collide(&p, subapp->collision))
            return true;
    }
    return false;
}
#endif

SubApplication * SubApplication::current = NULL;
int SubApplication::current_x = 0;
int SubApplication::current_y = 0;
