#include "shadercommon.h"
#include "glslshader.h"
#include "frameobject.h"
#include "manager.h"
#include "chowconfig.h"
#include "fileio.h"
#include "image.h"
#include "collision.h"
#include "datastream.h"
#include "assetfile.h"
#include "render.h"

BaseShader * BaseShader::current = NULL;

BaseShader::BaseShader(unsigned int id, int flags,
                       const char * texture_parameter)
: initialized(false), id(id), flags(flags),
  texture_parameter(texture_parameter)
{
}

static AssetFile fp;

void BaseShader::initialize()
{
    if (!fp.is_open())
        fp.open();

    fp.set_item(id, AssetFile::SHADER_DATA);

    program = glCreateProgramObject();
    GLhandleARB vert_shader = attach_source(fp, GL_VERTEX_SHADER_ARB);
    GLhandleARB frag_shader = attach_source(fp, GL_FRAGMENT_SHADER_ARB);

#ifndef CHOWDREN_USE_GL
    glBindAttribLocation(program, POSITION_ATTRIB_IDX, POSITION_ATTRIB_NAME);
    glBindAttribLocation(program, TEXCOORD1_ATTRIB_IDX, TEXCOORD1_ATTRIB_NAME);
    glBindAttribLocation(program, TEXCOORD2_ATTRIB_IDX, TEXCOORD2_ATTRIB_NAME);
    glBindAttribLocation(program, COLOR_ATTRIB_IDX, COLOR_ATTRIB_NAME);
#endif

    glLinkProgram(program);

    GLint status;
    glGetObjectParameteriv(program, GL_OBJECT_LINK_STATUS_ARB, &status);
    if (status == GL_FALSE) {
        GLint info_len;
        glGetObjectParameteriv(program, GL_OBJECT_INFO_LOG_LENGTH_ARB,
                               &info_len);
        GLchar * info_log = new GLchar[info_len + 1];
        glGetInfoLog(program, info_len, NULL, info_log);
        std::cout << "Linker failure: " << info_log << std::endl;
        delete[] info_log;
    }

    glDetachObject(program, vert_shader);
    glDetachObject(program, frag_shader);

    glUseProgramObject(program);

    // setup uniforms
    glUniform1i((GLint)get_uniform(TEXTURE_SAMPLER_NAME), 0);

    if (flags & SHADER_HAS_BACK)
        glUniform1i((GLint)get_uniform(BACKTEX_SAMPLER_NAME), 1);
    if (flags & SHADER_HAS_TEX_SIZE)
        size_uniform = (GLint)get_uniform(SIZE_UNIFORM_NAME);

    if (texture_parameter != NULL) {
        glUniform1i((GLint)get_uniform(texture_parameter), 2);
    }

    current = NULL;
    initialize_parameters();
    initialized = true;
}

void BaseShader::initialize_parameters()
{
}

GLhandleARB BaseShader::attach_source(FSFile & fp, GLenum type)
{
    GLhandleARB shader = glCreateShaderObject(type);

    FileStream stream(fp);
    size_t size = stream.read_uint32();
    GLchar * data = new GLchar[size];
    stream.read(data, size);

    GLint len = size;
    glShaderSource(shader, 1, (const GLchar**)&data, &len);
    delete[] data;
    glCompileShader(shader);

    GLint status;
    glGetObjectParameteriv(shader, GL_OBJECT_COMPILE_STATUS_ARB, &status);
    if (status == GL_FALSE) {
        GLint info_len;
        glGetObjectParameteriv(shader, GL_OBJECT_INFO_LOG_LENGTH_ARB,
                               &info_len);
        GLchar * info_log = new GLchar[info_len + 1];
        glGetInfoLog(shader, info_len, NULL, info_log);
        std::cout << "Compile error in " << type << ":" << std::endl <<
            info_log << std::endl;
        delete[] info_log;
    } else {
        glAttachObject(program, shader);
    }
    return shader;
}

void BaseShader::begin(FrameObject * instance, int width, int height)
{
    if (!initialized)
        initialize();

    if (flags & SHADER_HAS_BACK) {
        int box[4];
        instance->get_screen_aabb(box);
        Texture t = Render::copy_rect(box[0], box[1], box[2], box[3]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, t);
        glActiveTexture(GL_TEXTURE0);
    }

    if (current != this) {
        glUseProgramObject(program);
        current = this;
    }

    if (flags & SHADER_HAS_TEX_SIZE) {
        glUniform2f(size_uniform, 1.0f / width, 1.0f / height);
    }
}

void BaseShader::set_int(FrameObject * instance, int src, int uniform)
{
    int val = (int)instance->get_shader_parameter(src);
    glUniform1i((GLint)uniform, val);
}

void BaseShader::set_float(FrameObject * instance, int src, int uniform)
{
    glUniform1f((GLint)uniform, instance->get_shader_parameter(src));
}

void BaseShader::set_vec4(FrameObject * instance, int src, int uniform)
{
    int val = (int)instance->get_shader_parameter(src);
    float a, b, c, d;
    convert_vec4(val, a, b, c, d);
    glUniform4f((GLint)uniform, a, b, c, d);
}

void BaseShader::set_image(FrameObject * instance, int src)
{
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, (GLuint)instance->get_shader_parameter(src));
    glActiveTexture(GL_TEXTURE0);
}

int BaseShader::get_uniform(const char * value)
{
    return glGetUniformLocation(program, value);
}

#define EQ_REVERSE_SUBTRACT GL_FUNC_REVERSE_SUBTRACT
#define EQ_ADD GL_FUNC_ADD

#define FUNC_DST_COLOR GL_DST_COLOR
#define FUNC_ONE GL_ONE
#define FUNC_SRC_ALPHA GL_SRC_ALPHA
#define FUNC_ONE_MINUS_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
#define FUNC_ZERO GL_ZERO
#define FUNC_SRC_COLOR GL_SRC_COLOR

#define set_blend_eqs(a, b) glBlendEquationSeparate(a, b)
#define set_blend_eq(a) glBlendEquation(a)
#define set_blend_func(a, b) glBlendFunc(a, b)
#define commit_parameters(x)

#include "shadercommon.cpp"

void set_scale_uniform(float width, float height, float x_scale, float y_scale)
{
    glUniform1f(pixelscale_shader.x_scale, x_scale);
    glUniform1f(pixelscale_shader.y_scale, y_scale);
    glUniform1f(pixelscale_shader.x_size, width);
    glUniform1f(pixelscale_shader.y_size, height);
}
