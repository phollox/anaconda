#include "render.h"
#include "glslshader.h"

RenderData render_data;

float render_texcoords2[12] = {
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
    
    1.0f, 0.0f,
    0.0f, 0.0f,
    0.0f, 1.0f
};

void Render::init()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glEnableVertexAttribArray(POSITION_ATTRIB_IDX);
    // glVertexAttribPointer(POSITION_ATTRIB_IDX, 3, GL_FLOAT, GL_FALSE, 0,
    //                       (void*)&render_data.positions[0]);
    // glEnableVertexAttribArray(COLOR_ATTRIB_IDX);
    // glVertexAttribPointer(COLOR_ATTRIB_IDX, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0,
    //                       (void*)&render_data.colors[0]);

    // glVertexAttribPointer(TEXCOORD1_ATTRIB_IDX, 2, GL_FLOAT, GL_FALSE, 0,
    //                       (void*)&render_data.texcoords[0]);

    // glVertexAttribPointer(TEXCOORD2_ATTRIB_IDX, 2, GL_FLOAT, GL_FALSE, 0,
    //                       (void*)&render_data.texcoords[1]);

    glVertexPointer(2, GL_FLOAT, 0,
                    (void*)&render_data.positions[0]);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, (void*)&render_data.colors[0]);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glClientActiveTexture(GL_TEXTURE0);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, (void*)&render_data.texcoord1[0]); 

    glClientActiveTexture(GL_TEXTURE1);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, (void*)&render_texcoords2[0]);

    glClientActiveTexture(GL_TEXTURE0);

    unsigned int white = 0xFFFFFFFF;
    render_data.white_tex = Render::create_tex(&white, RGBA, 1, 1);

    glGenTextures(1, &render_data.back_tex);
    glBindTexture(GL_TEXTURE_2D, render_data.back_tex);
#ifdef CHOWDREN_QUICK_SCALE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#else
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#endif
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    render_data.last_tex = 0;
}
