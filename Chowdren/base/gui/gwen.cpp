#include "common.h"
#include "gwen.h"
#include "manager.h"

GwenData::GwenData() : renderer()
{
    renderer.Init();
    skin = new Gwen::Skin::Simple(); // For proper default colors, use empty ctor
    skin->SetRender(&renderer);
    canvas = new Gwen::Controls::Canvas(skin);
}

GwenData::~GwenData()
{
    delete skin;
    delete canvas;
}

void GwenData::render(Gwen::Controls::Base* control)
{
    // Update logic
    control->Think();
    // Set up layout (such as scroll bar buttons)
    control->RecurseLayout(gwen.skin);
    // Necessary to do manually here
    renderer.SetClipRegion(control->GetBounds());
    control->RenderRecursive(skin, control->GetBounds());
}



int get_state(InputList& il, int v)
{
    for (int i = 0; i < il.count; i++) {
        const InputList::InputState & s = il.states[i];
        if (s.key != v)
            continue;
        return s.state;
    }
    return -1;
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
    static int x, y;
    int dx = x, dy = y;
    manager.frame->get_mouse_pos(&x, &y);
    dx = x - dx;
    dy = y - dy;
    canvas->InputMouseMoved(x, y, dx, dy);

    // LMB
    int lmb = get_state(manager.mouse, SDL_BUTTON_LEFT);

    if (lmb == InputList::STATE_PRESSED)
        canvas->InputMouseButton(0, true);
    else if (lmb == InputList::STATE_RELEASED)
        canvas->InputMouseButton(0, false);

    // RMB
    int rmb = get_state(manager.mouse, SDL_BUTTON_LEFT);

    if (rmb == InputList::STATE_PRESSED)
        canvas->InputMouseButton(1, true);
    else if (rmb == InputList::STATE_RELEASED)
        canvas->InputMouseButton(1, false);

    // Keyboard
    for (int i = 0; i < manager.keyboard.count; i++) {
        const InputList::InputState & s = manager.keyboard.states[i];

        if (int gwen_key = get_gwen_key(s.key)) {
            if (s.state == InputList::STATE_PRESSED)
                canvas->InputKey(gwen_key, true);
            else if (s.state == InputList::STATE_RELEASED)
                canvas->InputKey(gwen_key, false);
        } else if (iswprint(s.key)) {
            if (s.state == InputList::STATE_PRESSED)
                canvas->InputCharacter(s.key);
        }
    }

    canvas->DoThink();
}

GwenData gwen;
