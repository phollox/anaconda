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

SubApplication::SubApplication(int x, int y, int id)
: FrameObject(x, y, id)
{
    current = this;
    subapp_frame.instances.clear();
#ifdef CHOWDREN_SUBAPP_FRAMES
    frames.push_back(this);
    collision = new InstanceBox(this);
#endif
}

SubApplication::~SubApplication()
{
#ifdef CHOWDREN_SUBAPP_FRAMES
    delete collision;
    frames.erase(std::remove(frames.begin(), frames.end(), this),
                 frames.end());
#endif

    Frame * old_frame = manager.frame;
    manager.frame = &subapp_frame;
    subapp_frame.data->on_app_end();
    manager.frame = old_frame;

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
}

void SubApplication::restart(int index)
{
    done = false;
    starting = true;
    subapp_frame.next_frame = index + frame_offset;
}

void SubApplication::update()
{
    if (done)
        return;
    starting = false;
    Frame * old_frame = manager.frame;
    manager.frame = &subapp_frame;

#ifdef CHOWDREN_USE_GWEN
    subapp_frame.gwen.update();
#endif

    if (subapp_frame.next_frame != -1) {
        int next_frame = subapp_frame.next_frame;
		std::cout << "Prepare new frame: " << this->index
            << " " << next_frame << std::endl;
        if (subapp_frame.index != -1)
            subapp_frame.on_end();
        std::cout << "Set frame: " << this->index <<
            " " << subapp_frame.index << " " << next_frame << std::endl;
        set_frame(next_frame);
    }

#ifdef CHOWDREN_SUBAPP_FRAMES
    width = subapp_frame.width;
    height = subapp_frame.height;
#endif

	std::cout << "Subapp update: " << this->index
        << " " << subapp_frame.index << std::endl;
    bool ret = subapp_frame.update();
	std::cout << "Subapp update done " << this->index
        << " " << subapp_frame.index << std::endl;

    if (!ret)
        subapp_frame.on_end();

    manager.frame = old_frame;

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

#ifdef CHOWDREN_SUBAPP_FRAMES
void SubApplication::draw_subapp()
{
    if (starting || done || !(flags & VISIBLE))
        return;
    Render::enable_scissor(x, y, width, height);
    subapp_frame.off_x = -x;
    subapp_frame.off_y = -y;
    subapp_frame.draw(0);
    Render::disable_scissor();
}

void SubApplication::draw_frames()
{
    vector<SubApplication*>::iterator it;
    for (it = frames.begin(); it != frames.end(); ++it) {
        (*it)->draw_subapp();
    }
}

bool SubApplication::test_pos(Frame * frame, int x, int y)
{
    PointCollision p(x, y);
    vector<SubApplication*>::iterator it;
    for (it = frames.begin(); it != frames.end(); ++it) {
        SubApplication * subapp = *it;
        if (collide(&p, subapp->collision))
            return true;
    }
    return false;
}
#endif

SubApplication * SubApplication::current = NULL;
