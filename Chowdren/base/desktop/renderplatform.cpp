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

#ifdef CHOWDREN_USE_D3D

static LPDIRECT3DVERTEXDECLARATION9 decl_instance = 0;

void reset_d3d_state()
{
    BaseShader::current = NULL;
    render_data.last_sampler = (D3DTEXTUREFILTERTYPE)-1;
    render_data.device->SetVertexDeclaration(decl_instance);
    render_data.device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
    render_data.device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    render_data.device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    render_data.device->SetRenderState(D3DRS_SRCBLEND,
                                       D3DBLEND_SRCALPHA);
    render_data.device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    render_data.device->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
    render_data.device->SetRenderState(D3DRS_DESTBLENDALPHA,
                                       D3DBLEND_INVSRCALPHA);
    render_data.device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
    render_data.device->SetRenderState(D3DRS_LIGHTING, FALSE);

    D3DMATRIX matrix;
    matrix.m[0][0] = 1.0f;
    matrix.m[0][1] = 0.0f;
    matrix.m[0][2] = 0.0f;
    matrix.m[0][3] = 0.0f;
    matrix.m[1][0] = 0.0f;
    matrix.m[1][1] = 1.0f;
    matrix.m[1][2] = 0.0f;
    matrix.m[1][3] = 0.0f;
    matrix.m[2][0] = 0.0f;
    matrix.m[2][1] = 0.0f;
    matrix.m[2][2] = 1.0f;
    matrix.m[2][3] = 0.0f;
    matrix.m[3][0] = 0.0f;
    matrix.m[3][1] = 0.0f;
    matrix.m[3][2] = 0.0f;
    matrix.m[3][3] = 1.0f;
    render_data.device->SetTransform(D3DTS_WORLD, &matrix);
    render_data.device->SetTransform(D3DTS_VIEW, &matrix);
    render_data.device->SetTransform(D3DTS_PROJECTION, &matrix);

    render_data.device->SetSamplerState(0, D3DSAMP_ADDRESSU,
                                        D3DTADDRESS_CLAMP);
    render_data.device->SetSamplerState(0, D3DSAMP_ADDRESSV,
                                        D3DTADDRESS_CLAMP);
    render_data.device->SetSamplerState(1, D3DSAMP_ADDRESSU,
                                        D3DTADDRESS_CLAMP);
    render_data.device->SetSamplerState(1, D3DSAMP_ADDRESSV,
                                        D3DTADDRESS_CLAMP);
}
#endif

void Render::init()
{
#ifdef CHOWDREN_USE_D3D
    D3DVERTEXELEMENT9 decl[] = {
        {0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,
         D3DDECLUSAGE_POSITION, 0},
        {0, 8, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT,
         D3DDECLUSAGE_COLOR, 0},
        {0, 8+4, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,
         D3DDECLUSAGE_TEXCOORD, 0},
        {0, 8+4+8, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,
         D3DDECLUSAGE_TEXCOORD, 1},
         D3DDECL_END()
    };
    render_data.device->CreateVertexDeclaration(decl, &decl_instance);

    reset_d3d_state();
#else
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

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
#endif

    unsigned int white = 0xFFFFFFFF;
    render_data.white_tex = Render::create_tex(&white, RGBA, 1, 1);
    render_data.last_tex = 0;
}

#ifdef CHOWDREN_USE_D3D

static void set_rgba_data(void * in_pixels, D3DLOCKED_RECT * rect,
                          int width, int height)
{
    int w_b = width * 4;
    if (rect->Pitch == w_b) {
        memcpy(rect->pBits, in_pixels, width * height * 4);
        return;
    }
    unsigned char * data = (unsigned char*)rect->pBits;
    unsigned char * pixels = (unsigned char*)in_pixels;
    for (int y = 0; y < height; y++) {
        memcpy(data, pixels, w_b);
        data += rect->Pitch;
        pixels += w_b;
    }
}

static void set_alpha_data(void * in_pixels, D3DLOCKED_RECT * rect,
                           int width, int height)
{
    if (rect->Pitch == width) {
        memcpy(rect->pBits, in_pixels, width * height);
        return;
    }
    unsigned char * data = (unsigned char*)rect->pBits;
    unsigned char * pixels = (unsigned char*)in_pixels;
    for (int y = 0; y < height; y++) {
        memcpy(data, pixels, width);
        data += rect->Pitch;
        pixels += width;
    }
}

Texture Render::create_tex(void * pixels, Format f, int width, int height)
{
    Texture tex;
    for (tex = 0; tex < MAX_TEX; ++tex) {
        if (render_data.textures[tex].texture == NULL)
            break;
    }

    if (tex == MAX_TEX)
        return 0;

    D3DFORMAT format;
    switch (f) {
        case RGBA:
            format = D3DFMT_A8R8G8B8;
            break;
        case L:
            format = D3DFMT_A8;
            break;
    }
    TextureData & t = render_data.textures[tex];
    render_data.device->CreateTexture(width, height, 1, 0, format,
                                      D3DPOOL_MANAGED, &t.texture, NULL);

    D3DLOCKED_RECT rect;
    t.texture->LockRect(0, &rect, NULL, 0);
    switch (f) {
        case RGBA:
            set_rgba_data(pixels, &rect, width, height);
            break;
        case L:
            set_alpha_data(pixels, &rect, width, height);
            break;
    }
    t.texture->UnlockRect(0);
    return tex;
}

#endif
