#include "objects/systembox.h"
#include "include_gl.h"
#include "collision.h"
#include <iostream>
#include "render.h"

SystemBox::SystemBox(int x, int y, int type_id)
: FrameObject(x, y, type_id)
{
    collision = new InstanceBox(this);
}

SystemBox::~SystemBox()
{
    delete collision;
}

void SystemBox::draw()
{
    int xx, yy;
    switch (type) {
        case PATTERN_IMAGE:
            Render::enable_scissor(x, y, width, height);
            for (xx = x; xx < x + width; xx += image->width)
            for (yy = y; yy < y + height; yy += image->height) {
                draw_image(image, xx + image->hotspot_x,
                           yy + image->hotspot_y, Color());
            }
			Render::disable_scissor();
            break;
        case CENTER_IMAGE:
            xx = x + width / 2 - image->width / 2;
            yy = y + height / 2 - image->height / 2;
            draw_image(image, xx + image->hotspot_x, yy + image->hotspot_y,
                       Color());
            break;
        case TOPLEFT_IMAGE:
            draw_image(image, x + image->hotspot_x, y + image->hotspot_y,
                       Color());
            break;
        default:
            break;
    }
}

void SystemBox::hide_border_1()
{
    // std::cout << "Hide System Box border 1 not implemented" << std::endl;
}

void SystemBox::hide_border_2()
{
    // std::cout << "Hide System Box border 1 not implemented" << std::endl;
}

void SystemBox::hide_fill()
{
    // std::cout << "Hide System Box fill not implemented" << std::endl;
}

void SystemBox::set_size(int w, int h)
{
    width = w;
    height = h;
    collision->update_aabb();
}

void SystemBox::set_text(const std::string & text)
{
    this->text = text;
}

void SystemBox::set_border_1(Color color)
{

}

void SystemBox::set_border_2(Color color)
{

}

void SystemBox::set_fill(Color color)
{

}

class DefaultInstance : public SystemBox
{
public:
    DefaultInstance()
    : SystemBox(0, 0, 0)
    {
        setup_default_instance(this);
        collision = new InstanceBox(this);
        create_alterables();
    }
};

static DefaultInstance default_systembox;
FrameObject * default_systembox_instance = &default_systembox;
