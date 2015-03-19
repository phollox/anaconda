#include "include_gl.h"
#include "shadercommon.h"
#include "mathcommon.h"

// XXX consider merging drawcalls
#define RENDER_BUFFER 1

struct RenderData
{
    Texture last_tex, white_tex, back_tex;
    int effect;
    float trans_x, trans_y;
    float pos_x, pos_y;
    int viewport[4];
    float positions[(RENDER_BUFFER * 3) * 6];
    unsigned int colors[RENDER_BUFFER * 6];
    float texcoord1[(RENDER_BUFFER * 2) * 6];
};

extern RenderData render_data;

inline void Render::set_offset(int x, int y)
{
    offset[0] = x;
    offset[1] = y;
    render_data.pos_x = x;
    render_data.pos_y = y;
}

inline void Render::set_view(int x, int y, int w, int h)
{
    glViewport(x, y, w, h);
    render_data.viewport[0] = x;
    render_data.viewport[1] = y;
    render_data.viewport[2] = w;
    render_data.viewport[3] = h;

    render_data.trans_x = 2.0f / w;
    render_data.trans_y = 2.0f / h;
}

inline void Render::clear(Color color)
{
    glClearColor(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f,
                 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

inline void Render::set_filter(Texture tex, bool linear)
{
    glBindTexture(GL_TEXTURE_2D, tex);
    if (linear) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
}

inline Texture Render::create_tex(void * pixels, Format f,
                                  int width, int height)
{
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    GLenum format;
    switch (f) {
        case RGBA:
            format = GL_RGBA;
            break;
        case L:
            format = GL_ALPHA;
            break;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return tex;
}

inline void Render::delete_tex(Texture tex)
{
    glDeleteTextures(1, &tex);
}

inline float transform_x(float x)
{
    return (x + render_data.pos_x) * render_data.trans_x - 1.0f;
}

inline float transform_y(float y)
{
    return 1.0f - (y + render_data.pos_y) * render_data.trans_y;
}

inline void insert_quad(float * p)
{
    float * pp = &render_data.positions[0];

    // triangle 1
    float x1 = transform_x(p[0]);
    *pp++ = x1;
    float y1 = transform_y(p[1]);
    *pp++ = y1;
    *pp++ = transform_x(p[2]);
    *pp++ = transform_y(p[3]);

    float x3 = transform_x(p[4]);
    float y3 = transform_y(p[5]);
    *pp++ = x3;
    *pp++ = y3;

    // triangle 2
    *pp++ = x3;
    *pp++ = y3;

    *pp++ = transform_x(p[6]);
    *pp++ = transform_y(p[7]);
    *pp++ = x1;
    *pp++ = y1;
}

inline void insert_quad(int x1, int y1, int x2, int y2)
{
    float fx1 = transform_x(x1);
    float fx2 = transform_x(x2);
    float fy1 = transform_y(y1);
    float fy2 = transform_y(y2);

    float * p = &render_data.positions[0];

    // 1
    *p++ = fx1; *p++ = fy1;
    *p++ = fx2; *p++ = fy1;
    *p++ = fx2; *p++ = fy2;

    // 2
    *p++ = fx2; *p++ = fy2;
    *p++ = fx1; *p++ = fy2;
    *p++ = fx1; *p++ = fy1;
}

inline void insert_color(Color c)
{
    unsigned int cc;
    // rely on endianness
    memcpy(&cc, &c, sizeof(Color));

    unsigned int * p = &render_data.colors[0];
    for (int i = 0; i < 6; ++i)
        *p++ = cc;
}

inline void insert_horizontal_color(Color c1, Color c2)
{
    unsigned int cc1, cc2;
    // rely on endianness
    memcpy(&cc1, &c1, sizeof(Color));
    memcpy(&cc2, &c2, sizeof(Color));

    unsigned int * p = &render_data.colors[0];
    *p++ = cc1;
    *p++ = cc2;
    *p++ = cc2;
    *p++ = cc2;
    *p++ = cc1;
    *p++ = cc1;
}

inline void insert_vertical_color(Color c1, Color c2)
{
    unsigned int cc1, cc2;
    // rely on endianness
    memcpy(&cc1, &c1, sizeof(Color));
    memcpy(&cc2, &c2, sizeof(Color));

    unsigned int * p = &render_data.colors[0];
    *p++ = cc1;
    *p++ = cc1;
    *p++ = cc2;

    *p++ = cc2;
    *p++ = cc2;
    *p++ = cc1;
}

const float render_texcoords[12] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,

    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f
};

inline void insert_texcoord1()
{
    memcpy(&render_data.texcoord1[0], &render_texcoords[0],
           sizeof(render_texcoords));
}

inline void insert_texcoord1(float fx1, float fy1, float fx2, float fy2)
{
    float * p = &render_data.texcoord1[0];

    // 1
    *p++ = fx1; *p++ = fy1;
    *p++ = fx2; *p++ = fy1;
    *p++ = fx2; *p++ = fy2;

    // 2
    *p++ = fx2; *p++ = fy2;
    *p++ = fx1; *p++ = fy2;
    *p++ = fx1; *p++ = fy1;
}

inline void begin_draw(Texture t)
{
    if (render_data.effect == Render::NONE)
        shader_set_texture();

    if (render_data.last_tex != t) {
        glBindTexture(GL_TEXTURE_2D, t);
        render_data.last_tex = t;
    }
}

inline void Render::draw_quad(int x1, int y1, int x2, int y2, Color c)
{
    draw_tex(x1, y1, x2, y2, c, render_data.white_tex);
}

inline void Render::draw_quad(float * p, Color c)
{
    draw_tex(p, c, render_data.white_tex);
}

inline void Render::draw_tex(int x1, int y1, int x2, int y2, Color c,
                             Texture t)
{
    begin_draw(t);

    insert_quad(x1, y1, x2, y2);
    insert_color(c);
    insert_texcoord1();
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

inline void Render::draw_tex(int x1, int y1, int x2, int y2, Color c,
                             Texture t,
                             float tx1, float ty1, float tx2, float ty2)
{
    begin_draw(t);

    insert_quad(x1, y1, x2, y2);
    insert_color(c);
    insert_texcoord1(tx1, ty1, tx2, ty2);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

inline void Render::draw_tex(float * p, Color c, Texture t)
{
    begin_draw(t);

    insert_color(c);
    insert_texcoord1();
    insert_quad(p);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

inline void Render::draw_horizontal_gradient(int x1, int y1, int x2, int y2,
                                             Color c1, Color c2)
{
    begin_draw(render_data.white_tex);

    insert_quad(x1, y1, x2, y2);
    insert_horizontal_color(c1, c2);
    insert_texcoord1();
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

inline void Render::draw_vertical_gradient(int x1, int y1, int x2, int y2,
                                           Color c1, Color c2)
{
    begin_draw(render_data.white_tex);

    insert_quad(x1, y1, x2, y2);
    insert_vertical_color(c1, c2);
    insert_texcoord1();
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

inline void Render::set_effect(int effect, FrameObject * obj,
                               int width, int height)
{
    render_data.effect = effect;
    shader_set_effect(effect, obj, width, height);
}

inline void Render::set_effect(int effect)
{
    render_data.effect = effect;
    shader_set_effect(effect, NULL, 0, 0);
}

inline void Render::disable_effect()
{
    render_data.effect = NONE;
}

inline Texture Render::copy_rect(int x1, int y1, int x2, int y2)
{
    int width = x2 - x1;
    int height = y2 - y1;

    int y = WINDOW_HEIGHT - y2;

    glBindTexture(GL_TEXTURE_2D, render_data.back_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
                 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, x1, y, width, height);
    return render_data.back_tex;
}

inline void Render::enable_blend()
{
    glEnable(GL_BLEND);
}

inline void Render::disable_blend()
{
    glDisable(GL_BLEND);
}

inline void Render::enable_scissor(int x, int y, int w, int h)
{
    glEnable(GL_SCISSOR_TEST);

    int w_x1 = int(x + offset[0]);
    int w_y2 = int(WINDOW_HEIGHT - y - offset[1]);
    int w_x2 = w_x1 + w;
    int w_y1 = w_y2 - h;

    w_x1 = int_max(0, int_min(w_x1, WINDOW_WIDTH));
    w_y1 = int_max(0, int_min(w_y1, WINDOW_HEIGHT));
    w_x2 = int_max(0, int_min(w_x2, WINDOW_WIDTH));
    w_y2 = int_max(0, int_min(w_y2, WINDOW_HEIGHT));

    glScissor(w_x1, w_y1, w_x2 - w_x1, w_y2 - w_y1);
}

inline void Render::disable_scissor()
{
    glDisable(GL_SCISSOR_TEST);
}