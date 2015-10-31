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
    if (window_control) {\
        current_x += start_x + 6;\
        current_y += start_y + 28;\
    } else {\
        current_x += get_render_x();\
        current_y += get_render_y();\
    }

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
    start_x = x;
    start_y = y;
#endif

#ifdef CHOWDREN_USE_GWEN
    window_control = NULL;
    gwen_close = false;
    subapp_flags = 0;
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
	if (window_control)
		window_control->DelayedDelete();
#endif

    SET_APP();
    if (!done && !starting) {
        subapp_frame.data->on_app_end(&subapp_frame);
        subapp_frame.data->on_end(&subapp_frame);
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
}

void SubApplication::restart(int index)
{
    done = false;
    starting = true;
    subapp_frame.next_frame = index + frame_offset;
}

void SubApplication::update()
{
#ifdef CHOWDREN_USE_GWEN
    subapp_frame.gwen.frame_base->SetHidden(done || starting);
    if (done) {
        destroy();
        return;
    }
#endif
    if (done)
        return;
    starting = false;

    SET_APP();

    if (subapp_frame.next_frame != -1) {
        int next_frame = subapp_frame.next_frame;
        if (subapp_frame.index != -1)
            subapp_frame.on_end();
        set_frame(next_frame);
    }

#ifdef CHOWDREN_SUBAPP_FRAMES
    width = subapp_frame.width;
    height = subapp_frame.height;
    subapp_frame.display_width = subapp_frame.width;
    subapp_frame.display_height = subapp_frame.height;

    if (window_control == NULL) {
        int display_width = manager.main_frame->display_width - 32;
        int display_height = manager.main_frame->display_height - 32;
        int change_x = get_render_x();
        int change_y = get_render_y();
        if (current_x + width >= display_width) {
            int off_x = (current_x + width) - display_width;
            change_x -= off_x;
            current_x -= off_x;
        }
        if (current_y + height >= display_height) {
            int off_y = (current_y + height) - display_height;
            change_y -= off_y;
            current_y -= off_y;
        }
        set_position(std::max(0, change_x), std::max(0, change_y));
    }
#endif

    bool ret = subapp_frame.update();

#ifdef CHOWDREN_USE_GWEN
    if (gwen_close)
        ret = false;
#endif

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
    set_visible(true);

    if (!has_ignore_controls) {
        ignore_controls = manager.ignore_controls;
        manager.ignore_controls = false;
        has_ignore_controls = true; 
    }
}

#ifdef CHOWDREN_USE_GWEN

class GwenWindow : public Gwen::Controls::WindowControl
{
public:
    SubApplication * parent;

    GWEN_CONTROL_INLINE(GwenWindow, Gwen::Controls::WindowControl)
    {
        onWindowClosed.Add(this, &GwenWindow::on_close);
    }

    void on_close()
    {
        parent->gwen_close = true;
    }

    void Render(Gwen::Skin::Base * skin)
    {
        Render::set_offset(0, 0);
        Gwen::Controls::WindowControl::Render(skin);
        parent->draw_subapp();

    }
};

int SubApplication::get_render_x()
{
    if (subapp_flags & IS_POPUP)
        return x;
    if (subapp_flags & IS_DOCKED)
        return 0;
    return x + layer->off_x - frame->off_x;
}

int SubApplication::get_render_y()
{
    if (subapp_flags & IS_POPUP)
        return y;
    if (subapp_flags & IS_DOCKED)
        return 0;
    return y + layer->off_y - frame->off_y;
}

void SubApplication::init_window()
{
    Gwen::Controls::Canvas * canvas = manager.main_frame->gwen.canvas;
    window_control = new GwenWindow(canvas);
    window_control->SetName("WindowBase");
    subapp_frame.gwen.frame_base->SetParent(window_control);
    ((GwenWindow*)window_control)->parent = this;
    window_control->SetPos(x + 200, y + 200);
    window_control->SetSize(width+12, height+35);
    window_control->DisableResizing();
}

void SubApplication::init_frame()
{
    Gwen::Controls::Canvas * canvas = manager.main_frame->gwen.canvas;
    subapp_frame.gwen.frame_base->SetParent(canvas);
}
#endif

#ifdef CHOWDREN_SUBAPP_FRAMES
void SubApplication::draw_subapp()
{
    if (starting || done || !(flags & VISIBLE))
        return;
    if (window_control) {
        Gwen::Point p = window_control->GetPos();
        start_x = p.x;
        start_y = p.y;
        window_control->SetSize(width+12, height+35);
        window_control->SetTitle(subapp_frame.gwen.title.c_str());
    } else {
        subapp_frame.gwen.frame_base->SetPos(get_render_x(), get_render_y());
    }
    width = subapp_frame.width;
    height = subapp_frame.height;
    subapp_frame.display_width = subapp_frame.width;
    subapp_frame.display_height = subapp_frame.height;
    subapp_frame.gwen.frame_base->SetSize(width, height);
    Render::set_offset(0, 0);
    Render::SavedViewport saved;
    SET_APP();
    Render::enable_scissor(current_x, current_y, width, height);
    subapp_frame.draw(0);
    RESTORE_APP();
    Render::disable_scissor();
    saved.restore();
    Render::set_offset(0, 0);
}

void SubApplication::draw_frames()
{
    vector<SubApplication*>::iterator it;
    for (it = frames.begin(); it != frames.end(); ++it) {
        if ((*it)->window_control != NULL)
            continue;
        (*it)->draw_subapp();
    }
}

bool SubApplication::test_pos(Frame * frame)
{
    if (Gwen::MouseFocus)
        return true;
    Gwen::Controls::Base * frame_base;
    frame_base = frame->gwen.frame_base;
    Gwen::Controls::Base * hover = Gwen::HoveredControl;
    if (frame_base == frame->gwen.canvas && hover != frame_base)
        return true;
    Gwen::Controls::Base * test = hover;
    while (test) {
        if (test == frame_base)
            return false;
        if (test->GetName() == "WindowBase")
            break;
        test = test->GetParent();
    }
    if (hover && !hover->Visible())
        return false;
    return true;
}
#endif

SubApplication * SubApplication::current = NULL;
int SubApplication::current_x = 0;
int SubApplication::current_y = 0;
