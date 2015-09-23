#ifndef CHOWDREN_GWEN_H
#define CHOWDREN_GWEN_H

#include "Gwen/Gwen.h"
#include "Gwen/Skins/Simple.h"
#include "Gwen/Renderers/Chowdren.h"
#include <string>

class GwenData
{
public:
    int m_x, m_y;

    std::string title;

    Gwen::Controls::Canvas * canvas;
    Gwen::Controls::Base * frame_base;

    GwenData();
    ~GwenData();

    // Handle animations, input etc.
    void update();

    // Called from every extension to render itself
    void render(Gwen::Controls::Base* control);
};

#endif // CHOWDREN_MANAGER_H
