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

#include "fbo.h"
#include "chowconfig.h"

Framebuffer * current_fbo = NULL;

#ifdef CHOWDREN_USE_D3D
Framebuffer * Framebuffer::fbos[Framebuffer::MAX_FBO];
#endif

Framebuffer::Framebuffer(int w, int h)
{
    tex = 0;
#ifdef CHOWDREN_USE_D3D
    fbo_index = -1;
#endif
    init(w, h);
}

Framebuffer::Framebuffer()
{
    tex = 0;
#ifdef CHOWDREN_USE_D3D
    fbo_index = -1;
#endif
}

Framebuffer::~Framebuffer()
{
    destroy();
}

void Framebuffer::destroy()
{
    if (tex == 0)
        return;
#ifdef CHOWDREN_USE_D3D
    fbos[fbo_index] = NULL;
    fbo->Release();
    TextureData & t = render_data.textures[tex];
    if (t.texture != NULL)
        t.texture->Release();
    fbo_index = -1;
#else
    if (render_data.last_tex == tex)
        render_data.last_tex = -1;
    glDeleteTextures(1, &tex);
    glDeleteFramebuffers(1, &fbo);
#endif
    tex = 0;
}

void Framebuffer::init(int w, int h)
{
    this->w = w;
    this->h = h;
#ifdef CHOWDREN_USE_D3D
    if (fbo_index == -1) {    
        for (fbo_index = 0; fbo_index < MAX_FBO; ++fbo_index) {
            if (fbos[fbo_index] != NULL)
                continue;
            fbos[fbo_index] = this;
            break;
        }

#ifndef NDEBUG
        if (fbo_index == MAX_FBO)
            std::cout << "Max FBO count reached" << std::endl;
#endif

        for (tex = 1; tex < MAX_TEX; ++tex) {
            if (render_data.textures[tex].texture == NULL)
                break;
        }
    }

    TextureData & t = render_data.textures[tex];
    t.texture = NULL;
    fbo = NULL;
    HRESULT hr = render_data.device->CreateTexture(w, h, 1,
                                                   D3DUSAGE_RENDERTARGET,
                                                   D3DFMT_A8R8G8B8,
                                                   D3DPOOL_DEFAULT,
                                                   &t.texture,
                                                   NULL);
#ifdef CHOWDREN_POINT_FILTER
    t.sampler = D3DTEXF_POINT;
#else
    t.sampler = D3DTEXF_LINEAR;
#endif

    t.texture->GetSurfaceLevel(0, &fbo);

#else
    // for fullscreen or window resize
    glGenTextures(1, &tex);
    set_tex(tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 NULL);
#ifdef CHOWDREN_POINT_FILTER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#else
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#endif
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenFramebuffers(1, &fbo);
    bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, tex, 0);
    unbind();
#endif
}

void Framebuffer::bind()
{
    old_fbo = current_fbo;
    current_fbo = this;
#ifdef CHOWDREN_USE_D3D
    render_data.device->SetRenderTarget(0, fbo);
#else
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    render_data.last_tex = -1;
#endif
}

void Framebuffer::unbind()
{
#ifdef CHOWDREN_USE_D3D
    if (old_fbo == NULL)
        render_data.device->SetRenderTarget(0, render_data.default_target);
    else
        render_data.device->SetRenderTarget(0, old_fbo->fbo);
#else
    if (old_fbo == NULL)
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    else
        glBindFramebuffer(GL_FRAMEBUFFER, old_fbo->fbo);
    render_data.last_tex = -1;
#endif
    current_fbo = old_fbo;
}

Texture Framebuffer::get_tex()
{
    return tex;
}
