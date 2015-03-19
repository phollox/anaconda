#include "include_gl.h"
#include "platform.h"
#include "frameobject.h"
#include "assetfile.h"
#include "render.h"

#include "shaderparam.cpp"
#include "shaderparam.h"

void convert_vec4(int val, float & a, float & b, float & c, float & d)
{
    a = (val & 0xFF) / 255.0f;
    b = ((val >> 8) & 0xFF) / 255.0f;
    c = ((val >> 16) & 0xFF) / 255.0f;
    d = ((val >> 24) & 0xFF) / 255.0f;
}

// shader implementations

#include "shaders.cpp"

#define DO_SHADER(S) S.begin(obj, width, height);\
                     S.set_parameters(obj);\
                     commit_parameters(obj);

#define HANDLE_SHADER(N, S) case Render::N:\
                                DO_SHADER(S);\
                                break

#define SET_BLEND_EQ(a, b) has_blend_eq = true;\
                           set_blend_eqs(a, b);
#define SET_BLEND_FUNC(a, b) has_blend_func = true;\
                             set_blend_func(a, b);

static bool has_blend_eq = false;
static bool has_blend_func = false;

void shader_set_texture()
{
    if (has_blend_func) {
        set_blend_func(FUNC_SRC_ALPHA, FUNC_ONE_MINUS_SRC_ALPHA);
        has_blend_func = false;
    }

    if (has_blend_eq) {
        set_blend_eq(EQ_ADD);
        has_blend_eq = false;
    }
    
    texture_shader.begin(NULL, 0, 0);
}

void shader_set_effect(int effect, FrameObject * obj,
                       int width, int height)
{
    if (has_blend_func) {
        set_blend_func(FUNC_SRC_ALPHA, FUNC_ONE_MINUS_SRC_ALPHA);
        has_blend_func = false;
    }

    if (has_blend_eq) {
        set_blend_eq(EQ_ADD);
        has_blend_eq = false;
    }

    switch (effect) {
        case Render::NONE:
            break;
        case Render::SUBTRACT:
            DO_SHADER(subtract_shader);
            SET_BLEND_EQ(EQ_REVERSE_SUBTRACT, EQ_ADD);
            SET_BLEND_FUNC(FUNC_DST_COLOR, FUNC_ONE);
            break;
        case Render::ADDITIVE:
            texture_shader.begin(NULL, 0, 0);
            SET_BLEND_FUNC(FUNC_SRC_ALPHA, FUNC_ONE);
            break;
        case Render::SURFACESUBTRACT:
            texture_shader.begin(NULL, 0, 0);
            SET_BLEND_EQ(EQ_REVERSE_SUBTRACT, EQ_ADD);
            SET_BLEND_FUNC(FUNC_ONE, FUNC_ONE);
            break;
        case Render::LAYERCOLOR:
            texture_shader.begin(NULL, 0, 0);
            SET_BLEND_FUNC(FUNC_ZERO, FUNC_SRC_COLOR);
            break;
        case Render::PIXELSCALE:
            pixelscale_shader.begin(NULL, 0, 0);
            break;
        case Render::FONT:
            font_shader.begin(NULL, 0, 0);
            break;
        HANDLE_SHADER(PERSPECTIVE, perspective_shader);
        HANDLE_SHADER(MONOCHROME, monochrome_shader);
        HANDLE_SHADER(ZOOMOFFSET, zoomoffset_shader);
        HANDLE_SHADER(PIXELOUTLINE, pixeloutline_shader);
        HANDLE_SHADER(COLDIRBLUR, coldirblur_shader);
        HANDLE_SHADER(CHANNELBLUR, channelblur_shader);
        HANDLE_SHADER(ROTATESUB, rotatesub_shader);
        HANDLE_SHADER(SUBPX, subpx_shader);
        HANDLE_SHADER(SIMPLEMASK, simplemask_shader);
        HANDLE_SHADER(MULTIPLY, multiply_shader);
        HANDLE_SHADER(BRIGHTSATBG, brightsatbg_shader);
        HANDLE_SHADER(NINEPATCH, ninepatch_shader);
        HANDLE_SHADER(HARDLIGHT, hardlight_shader);
        HANDLE_SHADER(UNDERWATER, underwater_shader);
        HANDLE_SHADER(LENS, lens_shader);
        HANDLE_SHADER(INVERT, invert_shader);
        HANDLE_SHADER(HUE, hue_shader);
        HANDLE_SHADER(TINT, tint_shader);
        HANDLE_SHADER(OVERLAYALPHA, overlayalpha_shader);
        HANDLE_SHADER(GRAIN, grain_shader);
        HANDLE_SHADER(GRADIENT, gradient_shader);
        HANDLE_SHADER(OFFSETSTATIONARY, offsetstationary_shader);
        HANDLE_SHADER(BGBLUR, bgblur_shader);
        HANDLE_SHADER(PATTERNOVERLAY, patternoverlay_shader);
        HANDLE_SHADER(BGBLOOM, bgbloom_shader);
        HANDLE_SHADER(MIXER, mixer_shader);
    }
}
