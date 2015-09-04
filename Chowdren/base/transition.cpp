#include "transition.h"
#include "manager.h"
#include "render.h"

void Transition::draw(int fade_type, float fade_value, Color color)
{
    switch (fade_type) {
        case FADE:
            color.set_alpha(int(fade_value * 255));
            Render::draw_quad(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, color);
            break;
        case DOOR:
            int w = (WINDOW_WIDTH / 2.0f) * fade_value;
            Render::draw_quad(0, 0, w, WINDOW_HEIGHT, color);
            Render::draw_quad(WINDOW_WIDTH - w, 0,
                              WINDOW_WIDTH, WINDOW_HEIGHT, color);
            break;
    }
}