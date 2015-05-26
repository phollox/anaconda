#include "objects/capture.h"
#include <iostream>
#include "fbo.h"
#include "platform.h"

void CaptureObject::set_filename(const std::string & value)
{
    filename = convert_path(value);
}

static Framebuffer fbo;

void CaptureObject::capture_frame()
{
    std::cout << "Capture frame" << std::endl;
}

void CaptureObject::on_capture()
{
    if (!capture)
        return;
    std::cout << "On capture!" << std::endl;
    capture = false;
    Texture t = Render::copy_rect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (fbo.tex == 0)
        fbo.init(WINDOW_WIDTH, WINDOW_HEIGHT);
    int offset[2] = {Render::offset[0], Render::offset[1]};
    Render::set_offset(0, 0);
    fbo.bind();

    float ty1;
    float ty2;
#ifdef CHOWDREN_FBO_FLIP
    ty1 = back_texcoords[1];
    ty2 = back_texcoords[5];
#else
    ty1 = back_texcoords[5];
    ty2 = back_texcoords[1];
#endif

    Render::draw_tex(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, Color(), t,
                     back_texcoords[0], ty1, back_texcoords[4], ty2);
    fbo.unbind();
    image.tex = fbo.get_tex();
    image.width = WINDOW_WIDTH;
    image.height = WINDOW_HEIGHT;
    Render::set_offset(offset[0], offset[1]);
}

void CaptureObject::capture_window()
{
    capture = true;
    std::cout << "Capture window" << std::endl;
}

void CaptureObject::set_size(int w, int h)
{
    std::cout << "Capture set size not implemented: " << w << " " << h
        << std::endl;
}

void CaptureObject::set_origin(int x, int y)
{
    std::cout << "Capture set pos not implemented: " << x << " " << y
        << std::endl;
}

std::string CaptureObject::filename;
Image CaptureObject::image;
bool CaptureObject::capture = false;
