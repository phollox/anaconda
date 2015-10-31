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

#ifndef CHOWDREN_RENDER_H
#define CHOWDREN_RENDER_H

#include "color.h"

typedef unsigned int Texture;

class FrameObject;

class Render
{
public:
    enum Effect
    {
        // application effects
        NONE = 0,
        MONOCHROME,
        ZOOMOFFSET,
        PIXELOUTLINE,
        FONTOUTLINE,
        COLDIRBLUR,
        CHANNELBLUR,
        CHANNELBLURADD,
        BLUR,
        BLURADD,
        ROTATESUB,
        SUBPX,
        SIMPLEMASK,
        MULTIPLY,
        BRIGHTSATBG,
        NINEPATCH,
        HARDLIGHT,
        UNDERWATER,
        LENS,
        SUBTRACT,
        INVERT,
        HUE,
        TINT,
        OVERLAYALPHA,
        GRAIN,
        GRADIENT,
        OFFSETSTATIONARY,
        DODGEBLUR,
        BGBLUR,
        PATTERNOVERLAY,
        BGBLOOM,
        ADDITIVE,
        OFFSET,
        MIXER,
        DISPLAY,
        LINEARBURN,
        LINEARDODGE,

        // internal effects
        SURFACESUBTRACT,
        LAYERCOLOR,
        PERSPECTIVE,
        PIXELSCALE,
        FONT,
        PREMUL
    };

    static int offset[2];
    static int viewport[4];

    static void init();

    static void set_view(int x, int y, int w, int h);
    static void set_offset(int x1, int y1);
    static void draw_quad(float * p, Color color);
    static void draw_quad(int x1, int y1, int x2, int y2, Color color);

    static void draw_horizontal_gradient(int x1, int y1, int x2, int y2,
                                         Color color1, Color color2);
    static void draw_vertical_gradient(int x1, int y1, int x2, int y2,
                                       Color color1, Color color2);

    static void draw_tex(int x1, int y1, int x2, int y2, Color color,
                         Texture tex);
    static void draw_tex(float * p, Color color, Texture tex);
    static void draw_tex(int x1, int y1, int x2, int y2, Color color,
                         Texture tex,
                         float tx1, float ty1, float tx2, float ty2);
    static void clear(Color color);

    static void enable_scissor(int x, int y, int w, int h);
    static void disable_scissor();

    static void clear(int r, int g, int b, int a)
    {
        clear(Color(r, g, b, a));
    }

    static void set_effect(int effect, FrameObject * obj,
                           int width, int height);
    static void set_effect(int effect);
    static void disable_effect();

    static void enable_blend();
    static void disable_blend();

    static Texture copy_rect(int x1, int y1, int x2, int y2);

    enum Format
    {
        RGBA,
        L
    };

    // textures
    static Texture create_tex(void * pixels, Format f, int width, int height);
    static void delete_tex(Texture tex);
    static void set_filter(Texture tex, bool linear);

#ifdef CHOWDREN_IS_3DS
    static void set_storage(bool vram);
    static bool is_vram_full();
    static void set_global_depth(float depth);
    static void set_depth(float depth);
#endif

    struct SavedOffset
    {
        int save[2];

        SavedOffset()
        {
            save[0] = Render::offset[0];
            save[1] = Render::offset[1];
        }

        void restore()
        {
            Render::set_offset(save[0], save[1]);
        }
    };

    struct SavedViewport
    {
        int save[4];

        SavedViewport()
        {
            save[0] = Render::viewport[0];
            save[1] = Render::viewport[1];
            save[2] = Render::viewport[2];
            save[3] = Render::viewport[3];
        }

        void restore()
        {
            Render::set_view(save[0], save[1], save[2], save[3]);
        }
    };

    struct SavedViewportOffset
    {
        SavedViewport viewport;
        SavedOffset offset;

        SavedViewportOffset()
        : offset(), viewport()
        {
        }

        void restore()
        {
            viewport.restore();
            offset.restore();
        }
    };
};

#include "renderplatform.h"

#endif // CHOWDREN_RENDER_H
