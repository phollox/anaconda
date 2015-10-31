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

#ifndef INCLUDE_GL_H
#define INCLUDE_GL_H

#if defined(CHOWDREN_USE_D3D) || defined(CHOWDREN_USE_GL) || \
    defined(CHOWDREN_USE_GLES2)

#ifdef CHOWDREN_USE_D3D

#ifndef NDEBUG
#define D3D_DEBUG_INFO
#endif

#include <d3d9.h>

#elif CHOWDREN_USE_GL
// SDL_opengl.h defines WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#include <SDL_opengl.h>

extern PFNGLBLENDEQUATIONSEPARATEEXTPROC __glBlendEquationSeparateEXT;
extern PFNGLBLENDEQUATIONEXTPROC __glBlendEquationEXT;
extern PFNGLBLENDFUNCSEPARATEEXTPROC __glBlendFuncSeparateEXT;
extern PFNGLACTIVETEXTUREARBPROC __glActiveTextureARB;
extern PFNGLCLIENTACTIVETEXTUREARBPROC __glClientActiveTextureARB;
extern PFNGLGENFRAMEBUFFERSEXTPROC __glGenFramebuffersEXT;
extern PFNGLDELETEFRAMEBUFFERSEXTPROC __glDeleteFramebuffersEXT;
extern PFNGLFRAMEBUFFERTEXTURE2DEXTPROC __glFramebufferTexture2DEXT;
extern PFNGLBINDFRAMEBUFFEREXTPROC __glBindFramebufferEXT;

extern PFNGLUSEPROGRAMOBJECTARBPROC __glUseProgramObjectARB;
extern PFNGLDETACHOBJECTARBPROC __glDetachObjectARB;
extern PFNGLGETINFOLOGARBPROC __glGetInfoLogARB;
extern PFNGLGETOBJECTPARAMETERIVARBPROC __glGetObjectParameterivARB;
extern PFNGLLINKPROGRAMARBPROC __glLinkProgramARB;
extern PFNGLCREATEPROGRAMOBJECTARBPROC __glCreateProgramObjectARB;
extern PFNGLATTACHOBJECTARBPROC __glAttachObjectARB;
extern PFNGLCOMPILESHADERARBPROC __glCompileShaderARB;
extern PFNGLSHADERSOURCEARBPROC __glShaderSourceARB;
extern PFNGLCREATESHADEROBJECTARBPROC __glCreateShaderObjectARB;
extern PFNGLUNIFORM1IARBPROC __glUniform1iARB;
extern PFNGLUNIFORM2FARBPROC __glUniform2fARB;
extern PFNGLUNIFORM1FARBPROC __glUniform1fARB;
extern PFNGLUNIFORM4FARBPROC __glUniform4fARB;
extern PFNGLGETUNIFORMLOCATIONARBPROC __glGetUniformLocationARB;

#define glBlendEquation __glBlendEquationEXT
#define glBlendEquationSeparate __glBlendEquationSeparateEXT
#define glBlendFuncSeparate __glBlendFuncSeparateEXT
#define glActiveTexture __glActiveTextureARB
#define glClientActiveTexture __glClientActiveTextureARB
#define glGenFramebuffers __glGenFramebuffersEXT
#define glDeleteFramebuffers __glDeleteFramebuffersEXT
#define glBindFramebuffer __glBindFramebufferEXT
#define glFramebufferTexture2D __glFramebufferTexture2DEXT

#define glUseProgramObject __glUseProgramObjectARB
#define glDetachObject __glDetachObjectARB
#define glGetInfoLog __glGetInfoLogARB
#define glGetObjectParameteriv __glGetObjectParameterivARB
#define glLinkProgram __glLinkProgramARB
#define glCreateProgramObject __glCreateProgramObjectARB
#define glAttachObject __glAttachObjectARB
#define glCompileShader __glCompileShaderARB
#define glShaderSource __glShaderSourceARB
#define glCreateShaderObject __glCreateShaderObjectARB
#define glUniform1i __glUniform1iARB
#define glUniform2f __glUniform2fARB
#define glUniform1f __glUniform1fARB
#define glUniform4f __glUniform4fARB
#define glGetUniformLocation __glGetUniformLocationARB

#elif CHOWDREN_USE_GLES1
#include <SDL_opengles.h>

#elif CHOWDREN_USE_GLES2

#include <SDL_opengles2.h>
#include "shadercommon.h"

#define glCreateProgramObject glCreateProgram
#define glCreateShaderObject glCreateShader
#define glGetInfoLog glGetShaderInfoLog
#define GL_VERTEX_SHADER_ARB GL_VERTEX_SHADER
#define GL_FRAGMENT_SHADER_ARB GL_FRAGMENT_SHADER
#define GL_OBJECT_COMPILE_STATUS_ARB GL_COMPILE_STATUS
#define GL_OBJECT_INFO_LOG_LENGTH_ARB GL_INFO_LOG_LENGTH
#define GL_OBJECT_LINK_STATUS_ARB GL_LINK_STATUS
#define glDetachObject glDetachShader
#define glAttachObject glAttachShader
#define glUseProgramObject glUseProgram
#define GLhandleARB GLuint

#endif // CHOWDREN_USE_GL

#undef TRANSPARENT

#endif

#endif // INCLUDE_GL_H
