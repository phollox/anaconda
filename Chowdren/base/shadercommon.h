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

#ifndef CHOWDREN_SHADERCOMMON_H
#define CHOWDREN_SHADERCOMMON_H

// GLES attrib indexes
#define MAX_ATTRIB 4
#define POSITION_ATTRIB_IDX 0
#define POSITION_ATTRIB_NAME "in_pos"
#define COLOR_ATTRIB_IDX 1
#define COLOR_ATTRIB_NAME "in_blend_color"
#define TEXCOORD1_ATTRIB_IDX 2
#define TEXCOORD1_ATTRIB_NAME "in_tex_coord1"
#define TEXCOORD2_ATTRIB_IDX 3
#define TEXCOORD2_ATTRIB_NAME "in_tex_coord2"
#define TEXTURE_SAMPLER_NAME "texture"
#define BACKTEX_SAMPLER_NAME "background_texture"
#define SIZE_UNIFORM_NAME "texture_size"

// shader flags
enum ShaderFlags
{
    SHADER_HAS_BACK = (1 << 0),
    SHADER_HAS_TEX_SIZE = (1 << 1)
};

class FrameObject;

void convert_vec4(int value, float & a, float & b, float & c, float & d);

void shader_set_effect(int effect, FrameObject * obj, int width, int height);
void shader_set_texture();

#endif // CHOWDREN_SHADERCOMMON_H
