#ifndef CHOWDREN_GWEN_H
#define CHOWDREN_GWEN_H

#include "Gwen/Gwen.h"
#include "Gwen/Skins/Simple.h"
#include "Gwen/Renderers/Chowdren.h"

class GwenData
{
public:
    Gwen::Renderer::Chowdren renderer;
    Gwen::Skin::Simple* skin;
    Gwen::Controls::Canvas* canvas;

    GwenData();
    ~GwenData();

    // Handle animations, input etc.
    void update();

    // Called from every extension to render itself
    void render(Gwen::Controls::Base* control);
};

extern GwenData gwen;

#endif // CHOWDREN_MANAGER_H
