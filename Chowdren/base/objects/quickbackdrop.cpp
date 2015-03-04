#include "objects/quickbackdrop.h"
#include "collision.h"
#include "image.h"
#include "common.h"
#include "chowconfig.h"

// QuickBackdrop

QuickBackdrop::QuickBackdrop(int x, int y, int type_id)
: FrameObject(x, y, type_id), image(NULL)
{
#ifdef CHOWDREN_LAYER_WRAP
    x_offset = y_offset = 0;
#endif
}

#ifdef CHOWDREN_LAYER_WRAP
void QuickBackdrop::set_backdrop_offset(int dx, int dy)
{
    x_offset = (x_offset + dx) % image->width;
    y_offset = (y_offset + dy) % image->height;
}
#endif

QuickBackdrop::~QuickBackdrop()
{
    delete collision;
}


inline int align_pos(int a, int b)
{
    if (a <= 0)
        return 0;
    return (a / b) * b;
}

void QuickBackdrop::draw()
{
    if (image != NULL) {
        int x = this->x;
        int y = this->y;
        int width = this->width;
        int height = this->height;

#ifdef CHOWDREN_LAYER_WRAP
        // this is a cheap implementation of the wrap feature.
        // we expect objects to extend on either the X or Y axis.
        if (layer->wrap_x) {
            x = frame->off_x * layer->coeff_x + x_offset - image->width;
            width = WINDOW_WIDTH + image->width * 2;
        } else if (layer->wrap_y) {
            y = frame->off_y * layer->coeff_y + y_offset - image->height;
            height = WINDOW_HEIGHT + image->height * 2;
        }
#endif
        int screen_x1 = x + layer->off_x - frame->off_x;
        int screen_y1 = y + layer->off_y - frame->off_y;
        int screen_x2 = screen_x1 + width;
        int screen_y2 = screen_y1 + height;

        int add_x = align_pos(0 - screen_x1, image->width);
        int add_y = align_pos(0 - screen_y1, image->height);
        x += add_x;
        width -= add_x;
        y += add_y;
        height -= add_y;

        width -= align_pos(screen_x2 - WINDOW_WIDTH, image->width);
        height -= align_pos(screen_y2 - WINDOW_HEIGHT, image->height);

#ifdef CHOWDREN_IS_3DS
        image->upload_texture();
        // can't use scissor for stereo 3d
        short image_width = image->width;
        short image_height = image->height;

        int full_width = (width / image_width) * image_width;
        int full_height = (height / image_height) * image_height;

        int end_x = x + width;
        int end_y = y + height;
        int full_end_x = x + full_width;
        int full_end_y = y + full_height;

        for (int xx = x; xx < end_x; xx += image_width)
        for (int yy = y; yy < end_y; yy += image_height) {
            if (xx >= full_end_x)
                image->width = end_x - xx;
            if (yy >= full_end_y)
                image->height = end_y - yy;
            draw_image(image, xx + image->hotspot_x, yy + image->hotspot_y);
            image->width = image_width;
            image->height = image_height;
        }
#else
		Render::enable_scissor(x, y, width, height);
        for (int xx = x; xx < x + width; xx += image->width)
        for (int yy = y; yy < y + height; yy += image->height) {
            draw_image(image, xx + image->hotspot_x, yy + image->hotspot_y, blend_color);
        }
		Render::disable_scissor();
#endif
    } else {
        begin_draw();
        int x1 = x;
        int y1 = y;
        int x2 = x + width;
        int y2 = y + height;
        if (outline > 0) {
            Render::draw_quad(x1, y1, x2, y2, outline_color);
            x1 += outline;
            y1 += outline;
            x2 -= outline;
            y2 -= outline;
        }

        draw_gradient(x1, y1, x2, y2, gradient_type, color, color2,
                      blend_color.a);
        end_draw();
    }
}
