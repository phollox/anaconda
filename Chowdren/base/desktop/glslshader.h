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

#ifndef CHOWDREN_GLSLSHADER_H
#define CHOWDREN_GLSLSHADER_H

#include "include_gl.h"
#include "fileio.h"

class FrameObject;

class BaseShader
{
public:
    static BaseShader * current;

#ifdef CHOWDREN_USE_D3D
    int tex_sampler;
    int back_sampler;
    int tex_param_sampler;
    int size_uniform;
    IDirect3DVertexShader9 * vert_shader;
    IDirect3DPixelShader9 * frag_shader;
#else
    GLhandleARB program;
    GLint size_uniform;
    GLhandleARB attach_source(FSFile & fp, GLenum type);
#endif
    bool initialized;
    unsigned int id;
    int flags;
    const char * texture_parameter;

    BaseShader(unsigned int id, int flags = 0,
               const char * texture_parameter = NULL);
    void initialize();
    int get_uniform(const char * value);
    virtual void initialize_parameters();
    void begin(FrameObject * instance, int width, int height);
    static void set_int(FrameObject * instance, int src, int uniform);
    static void set_float(FrameObject * instance, int src, int uniform);
    static void set_vec4(FrameObject * instance, int src, int uniform);
    static void set_image(FrameObject * instance, int src);
};

void set_scale_uniform(float width, float height,
                       float x_scale, float y_scale);

#endif // CHOWDREN_GLSLSHADER_H
