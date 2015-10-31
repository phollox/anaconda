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

#ifndef CHOWDREN_FBO_H
#define CHOWDREN_FBO_H

#include "include_gl.h"
#include "render.h"

class Framebuffer;
extern Framebuffer * current_fbo;

class Framebuffer
{
public:
    int w, h;
    Texture tex;
#ifdef CHOWDREN_USE_D3D
    enum {
        MAX_FBO = 512
    };

    IDirect3DSurface9 * fbo;
    int fbo_index;
    static Framebuffer * fbos[MAX_FBO];
#else
    GLuint fbo;
#endif
    Framebuffer * old_fbo;

    Framebuffer(int w, int h);
    Framebuffer();
    ~Framebuffer();
    void init(int w, int h);
    void destroy();
    void bind();
    void unbind();
    Texture get_tex();
};

#endif // CHOWDREN_FBO_H
