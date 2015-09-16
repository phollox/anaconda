#include "common.h"
#include "gwen.h"
#include "manager.h"

static bool initialized = false;
Gwen::Renderer::Chowdren renderer;
Gwen::Skin::Simple* skin;

GwenData::GwenData()
{
    if (!initialized) {
        initialized = true;
        renderer.Init();
        // For proper default colors, use empty ctor
        skin = new Gwen::Skin::Simple();
        skin->SetRender(&renderer);
    }
    canvas = new Gwen::Controls::Canvas(skin);
}

GwenData::~GwenData()
{
    delete canvas;
}

void GwenData::render(Gwen::Controls::Base* control)
{
    // Update logic
    control->Think();
    // Set up layout (such as scroll bar buttons)
    control->RecurseLayout(skin);
    // Necessary to do manually here
    renderer.SetClipRegion(control->GetBounds());
    control->RenderRecursive(skin, control->GetBounds());
}

int get_gwen_key(int key)
{
    static int translations[][2] =  {
        //{SDLK_RETURN, Gwen::Key::Return},
        {SDLK_BACKSPACE, Gwen::Key::Backspace},
        {SDLK_DELETE, Gwen::Key::Delete},
        {SDLK_LEFT, Gwen::Key::Left},
        {SDLK_RIGHT, Gwen::Key::Right},
        {SDLK_LSHIFT, Gwen::Key::Shift},
        {SDLK_RSHIFT, Gwen::Key::Shift},
        {SDLK_TAB, Gwen::Key::Tab},
        //{SDLK_SPACE, Gwen::Key::Space},
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
    // Cursor
    int dx = m_x, dy = m_y;
    manager.frame->get_mouse_pos(&m_x, &m_y);
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
        canvas->InputCharacter(*it);
    }

    canvas->DoThink();
}
