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

#include "common.h"
#include "gwen.h"
#include "manager.h"

static bool initialized = false;
Gwen::Renderer::Chowdren renderer;
Gwen::Skin::Simple* skin;

static Gwen::Controls::Canvas * global_canvas = NULL;

GwenData::GwenData()
{
    if (!initialized) {
        initialized = true;
        renderer.Init();
        // For proper default colors, use empty ctor
        skin = new Gwen::Skin::Simple();
        skin->SetRender(&renderer);
        global_canvas = new Gwen::Controls::Canvas(skin);
        global_canvas->SetName("WindowBase");
        canvas = global_canvas;
        frame_base = global_canvas;
    } else {
        frame_base = new Gwen::Controls::Base(global_canvas);
        frame_base->SetName("WindowBase");
		canvas = global_canvas;
    }
}

GwenData::~GwenData()
{
    if (frame_base != global_canvas)
        delete frame_base;
}

void GwenData::render(Gwen::Controls::Base* control)
{
    // // Update logic
    // control->Think();
    // // Set up layout (such as scroll bar buttons)
    // control->RecurseLayout(skin);
    // // Necessary to do manually here
    // renderer.SetClipRegion(control->GetBounds());
    // control->RenderRecursive(skin, control->GetBounds());
}

int get_gwen_key(int key)
{
    static int translations[][2] =  {
        {SDLK_RETURN, Gwen::Key::Return},
        {SDLK_BACKSPACE, Gwen::Key::Backspace},
        {SDLK_DELETE, Gwen::Key::Delete},
        {SDLK_LEFT, Gwen::Key::Left},
        {SDLK_RIGHT, Gwen::Key::Right},
        {SDLK_LSHIFT, Gwen::Key::Shift},
        {SDLK_RSHIFT, Gwen::Key::Shift},
        {SDLK_TAB, Gwen::Key::Tab},
        {SDLK_SPACE, Gwen::Key::Space},
        {SDLK_HOME, Gwen::Key::Home},
        {SDLK_END, Gwen::Key::End},
        {SDLK_LCTRL, Gwen::Key::Control},
        {SDLK_RCTRL, Gwen::Key::Control},
        {SDLK_UP, Gwen::Key::Up},
        {SDLK_DOWN, Gwen::Key::Down},
        {SDLK_ESCAPE, Gwen::Key::Escape},
        {SDLK_LALT, Gwen::Key::Alt},
        {SDLK_RALT, Gwen::Key::Alt},
    };

    for (int i = 0; i < 16; ++i) {
        if (translations[i][0] == key)
            return translations[i][1];
    }

    return 0;

}

void GwenData::update()
{
    if (manager.frame != manager.main_frame)
        return;
    canvas->SetSize(manager.frame->display_width,
                    manager.frame->display_height);
    Gwen::GlobalHoveredControl = NULL;

    int dx = m_x, dy = m_y;
    m_x = manager.mouse_x;
    m_y = manager.mouse_y;
    dx = m_x - dx;
    dy = m_y - dy;
    canvas->InputMouseMoved(m_x, m_y, dx, dy);

    if (is_mouse_pressed_once(SDL_BUTTON_LEFT))
        canvas->InputMouseButton(0, true);
    else if (is_mouse_released_once(SDL_BUTTON_LEFT))
        canvas->InputMouseButton(0, false);

    if (is_mouse_pressed_once(SDL_BUTTON_RIGHT))
        canvas->InputMouseButton(1, true);
    else if (is_mouse_released_once(SDL_BUTTON_RIGHT))
        canvas->InputMouseButton(1, false);

    // Keyboard
    for (int i = 0; i < manager.keyboard.count; i++) {
        const InputList::InputState & s = manager.keyboard.states[i];

        if (int gwen_key = get_gwen_key(s.key)) {
            if (s.state == InputList::STATE_PRESSED)
                canvas->InputKey(gwen_key, true);
            else if (s.state == InputList::STATE_RELEASED)
                canvas->InputKey(gwen_key, false);
        }
    }

    std::string::const_iterator it;
    for (it = manager.input.begin(); it != manager.input.end(); ++it) {
        switch (*it) {
            case '\n':
            case '\r':
                continue;
        }
        canvas->InputCharacter(*it);
    }
}
