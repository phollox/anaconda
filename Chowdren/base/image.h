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

#ifndef CHOWDREN_IMAGE_H
#define CHOWDREN_IMAGE_H

#include "path.h"
#include <string>
#include "color.h"
#include "types.h"
#include "platform.h"
#include "chowconfig.h"
#include "bitarray.h"
#include "render.h"

extern const float back_texcoords[4];
extern const float fbo_texcoords[4];

class Image
{
public:
    enum Flags
    {
        USED = 1 << 0,
        FILE = 1 << 1,
        CACHED = 1 << 2,
        STATIC = 1 << 3,
        KEEP = 1 << 4,
        LINEAR_FILTER = 1 << 5,
#ifdef CHOWDREN_QUICK_SCALE
        DEFAULT_FLAGS = 0
#else
        DEFAULT_FLAGS = LINEAR_FILTER
#endif
    };

    unsigned short handle;
    unsigned short flags;
    short hotspot_x, hotspot_y, action_x, action_y;
    short width, height;
    Texture tex;
    unsigned char * image;
#ifndef CHOWDREN_IS_WIIU
    BitArray alpha;
#endif

    Image();
    Image(int hot_x, int hot_y, int act_x, int act_y);
    Image(int handle);
    ~Image();
    void destroy();
    Image * copy();
    void replace(const Color & from, const Color & to);
    void replace(const std::string & path);
    void load();
    void set_static();
    void upload_texture();
    void draw(int x, int y, Color color, float angle = 0.0f,
              float scale_x = 1.0f, float scale_y = 1.0f);
    void draw_flip_x(int x, int y, Color color, float angle = 0.0f,
                     float scale_x = 1.0f, float scale_y = 1.0f);
    void draw(int x, int y, int src_x, int src_y, int w, int h, Color color);
    bool is_valid();
    void unload();
    void set_filter(bool linear);
    void set_transparent_color(TransparentColor color);

    // inline methods

    bool get_alpha(int x, int y)
    {
    #ifdef CHOWDREN_IS_WIIU
        if (tex != 0) {
            unsigned int v = platform_get_texture_pixel(tex, x, y);
            unsigned char c = ((unsigned char*)&v)[3];
            return c != 0;
        }
    #else
        if (alpha.data != NULL)
            return alpha.get(y * width + x) != 0;
    #endif
        unsigned int * v = (unsigned int*)image + y * width + x;
        unsigned char c = ((unsigned char*)v)[3];
        return c != 0;
    }
};

class FileImage : public Image
{
public:
    std::string filename;
    TransparentColor transparent;

    FileImage(const std::string & filename, int hot_x, int hot_y,
              int act_x, int act_y, TransparentColor transparent);
    void load_file();
    void load_data(unsigned char * data, int size);
};

Image * get_internal_image(unsigned int i);
Image * get_image_cache(const std::string & filename, int hot_x, int hot_y,
                        int act_x, int act_y, TransparentColor color);
bool has_image_cache(const std::string & filename); 
void set_image_cache(const std::string & filename, FileImage * image);

void reset_image_cache();
void flush_image_cache();
void preload_images();

extern Image dummy_image;

// image replacer

typedef std::pair<Color, Color> Replacement;

#define MAX_COLOR_REPLACE 10

struct ReplacedImage
{
    Image * src_image;
    Image * image;
    int hash;

    ReplacedImage(Image * src_image, Image * image, int hash)
    : src_image(src_image), image(image), hash(hash)
    {
    }
};

class ReplacedImages
{
public:
    int index;
    Replacement colors[MAX_COLOR_REPLACE];
    static vector<ReplacedImage> images;

    ReplacedImages()
    : index(0)
    {
    }

    void replace(Color from, Color to);
    void set_transparent(TransparentColor value);
    Image * apply(Image * image, Image * src_image);
    Image * apply_direct(Image * image, Image * src_image);

    bool empty()
    {
        return index <= 0;
    }
};

#endif // CHOWDREN_IMAGE_H
