#include <string>
#include "image.h"
#include "string.h"
#include "color.h"
#include <iostream>
#include <stdio.h>
#include "datastream.h"
#include "chowconfig.h"
#include "types.h"
#include "mathcommon.h"
#include "assetfile.h"
#include "render.h"

#define STBI_NO_STDIO
#define STBI_NO_HDR
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_ONLY_BMP
#include "stb_image.h"

inline unsigned char * load_png_image(FSFile & image_file, int size,
                                      int * w, int * h, int * channels)
{
    unsigned char * buf = new unsigned char[size];
    image_file.read(buf, size);
    unsigned char * out = stbi_load_from_memory(buf, size, w, h, channels, 4);
    delete[] buf;
    return out;
}

#ifdef CHOWDREN_USE_WEBP

inline unsigned char * load_image(FSFile & image_file, int size,
                                  int * w, int * h, int * channels)
{
    return load_png_image(image_file, size, w, h, channels);
}

#else

inline unsigned char * load_image(FSFile & image_file, int size,
                                  int * w, int * h, int * channels)
{
    return load_png_image(image_file, size, w, h, channels);
}

#endif

typedef vector<Image*> ImageList;

static AssetFile image_file;

void open_image_file()
{
    if (image_file.is_open())
        return;
    image_file.open();
}

// dummy constructor
Image::Image()
: handle(0), flags(DEFAULT_FLAGS), tex(0), image(NULL), width(0), height(0),
  hotspot_x(0), hotspot_y(0), action_x(0), action_y(0)
{
}

Image::Image(int hot_x, int hot_y, int act_x, int act_y)
: handle(0), flags(DEFAULT_FLAGS), tex(0), image(NULL), width(0), height(0),
  hotspot_x(hot_x), hotspot_y(hot_y), action_x(act_x), action_y(act_y)
{
}

Image::Image(int handle)
: handle(handle), tex(0), image(NULL), flags(DEFAULT_FLAGS)
{
}

Image::~Image()
{
    unload();
}

void Image::destroy()
{
    if (flags & CACHED)
        return;
    delete this;
}

Image * Image::copy()
{
    Image * new_image;
    if (image == NULL) {
        if (flags & FILE) {
            FileImage * image = (FileImage*)this;
            new_image = new FileImage(image->filename,
                                      image->hotspot_x, image->hotspot_y,
                                      image->action_x, image->action_y,
                                      image->transparent);
        } else {
            new_image = new Image(handle);
        }
        new_image->load();
        return new_image;
    }
    new_image = new Image();
    new_image->width = width;
    new_image->height = height;
    new_image->handle = handle;
    int size = width*height*4;
    new_image->image = (unsigned char*)malloc(size);
    memcpy(new_image->image, image, size);
    return new_image;
}

void Image::set_static()
{
    flags |= STATIC;
}

void Image::load()
{
    flags |= USED;

    if (tex != 0 || image != NULL)
        return;

    if (flags & FILE) {
        ((FileImage*)this)->load_file();
        return;
    }

    open_image_file();
    image_file.set_item(handle, AssetFile::IMAGE_DATA);
    FileStream stream(image_file);

    hotspot_x = stream.read_int16();
    hotspot_y = stream.read_int16();
    action_x = stream.read_int16();
    action_y = stream.read_int16();

    int size = stream.read_uint32();

    int w, h, channels;
    image = load_image(image_file, size, &w, &h, &channels);

    width = w;
    height = h;

    if (image == NULL) {
        std::cout << "Could not load image " << handle << std::endl;
        std::cout << stbi_failure_reason() << std::endl;
        return;
    }
}

void Image::unload()
{
    if (image != NULL)
        stbi_image_free(image);
    if (tex != 0)
        glDeleteTextures(1, &tex);
    image = NULL;
    tex = 0;

#ifndef CHOWDREN_IS_WIIU
    free(alpha.data);
    alpha.data = NULL;
#endif
}

void Image::replace(const Color & from, const Color & to)
{
    if (image == NULL) {
        std::cout << "Could not replace color in unloaded image" << std::endl;
        return;
    }
    for (int i = 0; i < width * height; i++) {
        unsigned char * c = &image[i*4];
        if (c[0] != from.r || c[1] != from.g || c[2] != from.b)
            continue;
        c[0] = to.r;
        c[1] = to.g;
        c[2] = to.b;
    }
}

void Image::upload_texture()
{
    if (tex != 0 || image == NULL)
        return;

#ifndef CHOWDREN_IS_WIIU
    // create alpha mask
    int size = width * height;
    BaseBitArray::word_t * data;
    data = (BaseBitArray::word_t*)malloc(GET_BITARRAY_SIZE(size) * 4);
    int i = 0;
    int ii = 0;
    unsigned char c;

    while (i < size) {
        BaseBitArray::word_t word = 0;
        for (BaseBitArray::word_t m = 1; m != 0; m <<= 1) {
            c = ((unsigned char*)(((unsigned int*)image) + i))[3];
            if (c != 0)
                word |= m;
            ++i;
            if (i >= size)
                break;
        }
        data[ii++] = word;
    }

    alpha.data = data;
#endif
    int gl_width, gl_height;

#ifdef CHOWDREN_NO_NPOT
    pot_w = std::max(8, round_pow2(width));
    pot_h = std::max(8, round_pow2(height));

    if (pot_w >= 1024 || pot_h >= 1024) {
        pot_w = std::min<short>(1024, pot_w);
        pot_h = std::min<short>(1024, pot_h);
    }

    gl_width = pot_w;
    gl_height = pot_h;

    if (pot_w != width || pot_h != height) {
        unsigned int * old_image =(unsigned int*)image;
        image = (unsigned char*)malloc(pot_w * pot_h * 4);
        unsigned int * image_arr = (unsigned int*)image;

        // in case the image is being cut off due to dimension restrictions
        int ww = std::min(width, pot_w);
        int hh = std::min(height, pot_h);

        for (int x = 0; x < ww; x++)
        for (int y = 0; y < hh; y++) {
            image_arr[x + y * pot_w] = old_image[x + y * width];
        }

        stbi_image_free(old_image);
    }
#else
    gl_width = width;
    gl_height = height;
#endif

    tex = Render::create_tex(image, Render::RGBA, gl_width, gl_height);

    Render::set_filter(tex, (flags & LINEAR_FILTER) != 0);

    if (flags & KEEP)
        return;

    // for memory reasons, we delete the image and access the alpha or
    // the texture directly
    stbi_image_free(image);
    image = NULL;
}

void Image::set_filter(bool linear)
{
    if (((flags & LINEAR_FILTER) != 0) == linear)
        return;
    if (linear)
        flags |= LINEAR_FILTER;
    else
        flags &= ~LINEAR_FILTER;
    if (tex == 0)
        return;

    Render::set_filter(tex, linear);
}

const float flipped_texcoords[8] = {
    1.0f, 0.0f,
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f
};

const float normal_texcoords[8] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f
};

#ifndef CHOWDREN_IS_WIIU
// XXX change glc_copy_color_buffer_rect so this isn't necessary

const float back_texcoords[8] = {
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f
};
#else
const float back_texcoords[8] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f
};
#endif

void Image::draw(int x, int y, Color color,
                 float angle, float scale_x, float scale_y)
{
    if (tex == 0) {
        upload_texture();

        if (tex == 0)
            return;
    }

    if (angle == 0.0f && scale_x == 1.0f && scale_y == 1.0f) {
        int xx = x - hotspot_x;
        int yy = y - hotspot_y;
        Render::draw_tex(xx, yy, xx + width, yy + height, color, tex);
        return;
    }

    float r = rad(angle);
    float c = cos(r);
    float s = sin(r);

    float x1 = -hotspot_x * scale_x;
    float y1 = -hotspot_y * scale_y;
    float x2 = (-hotspot_x + width) * scale_x;
    float y2 = (-hotspot_y + height) * scale_y;
    
    float x1c = x1 * c;
    float x1s = -x1 * s;
    float y1c = y1 * c;
    float y1s = y1 * s;
    float x2c = x2 * c;
    float x2s = -x2 * s;
    float y2c = y2 * c;
    float y2s = y2 * s;

    float p[8] = {
        x1c + y1s + x, x1s + y1c + y,
        x2c + y1s + x, x2s + y1c + y,
        x2c + y2s + x, x2s + y2c + y,
        x1c + y2s + x, x1s + y2c + y
    };
    Render::draw_tex(&p[0], color, tex);
}

void Image::draw_flip_x(int x, int y, Color color,
                        float angle, float scale_x, float scale_y)
{
    if (tex == 0) {
        upload_texture();

        if (tex == 0)
            return;
    }

    if (angle == 0.0f && scale_x == 1.0f && scale_y == 1.0f) {
        int xx = x - hotspot_x;
        int yy = y - hotspot_y;
        Render::draw_tex(xx + width, yy, xx, yy + height, color, tex);
        return;
    }

    float r = rad(angle);
    float c = cos(r);
    float s = sin(r);

    float x2 = -hotspot_x * scale_x;
    float y1 = -hotspot_y * scale_y;
    float x1 = (-hotspot_x + width) * scale_x;
    float y2 = (-hotspot_y + height) * scale_y;
    
    float x1c = x1 * c;
    float x1s = -x1 * s;
    float y1c = y1 * c;
    float y1s = y1 * s;
    float x2c = x2 * c;
    float x2s = -x2 * s;
    float y2c = y2 * c;
    float y2s = y2 * s;

    float p[8] = {
        x1c + y1s + x, x1s + y1c + y,
        x2c + y1s + x, x2s + y1c + y,
        x2c + y2s + x, x2s + y2c + y,
        x1c + y2s + x, x1s + y2c + y
    };
    Render::draw_tex(&p[0], color, tex);
}

void Image::draw(int x, int y, int src_x, int src_y, int w, int h, Color c)
{
    if (tex == 0) {
        upload_texture();

        if (tex == 0)
            return;
    }

    int x2 = x + w;
    int y2 = y + h;

    float t_x1 = float(src_x) / float(width);
    float t_x2 = t_x1 + float(w) / float(width);
    float t_y1 = float(src_y) / float(height);
    float t_y2 = t_y1 + float(h) / float(height);
    Render::draw_tex(x, y, x2, y2, c, tex, t_x1, t_y1, t_x2, t_y2);
}

bool Image::is_valid()
{
    return image != NULL || tex != 0;
}

// FileImage

FileImage::FileImage(const std::string & filename, int hot_x, int hot_y,
                     int act_x, int act_y, TransparentColor color)
: filename(filename), transparent(color), Image(hot_x, hot_y, act_x, act_y)
{
    flags |= FILE;
}

void FileImage::load_file()
{
    FSFile fp(filename.c_str(), "r");

    if (!fp.is_open()) {
        printf("Could not open image \"%s\"\n", filename.c_str());
        return;
    }

    int w, h, channels;
    image = load_png_image(fp, fp.get_size(), &w, &h, &channels);

    width = w;
    height = h;

    fp.close();

    if (image == NULL) {
        printf("Could not load image \"%s\"\n", filename.c_str());
        return;
    }

    if (!transparent.is_enabled())
        return;

#ifndef CHOWDREN_FORCE_TRANSPARENT
    if (channels != 1 && channels != 3)
        return;
#endif

    for (int i = 0; i < width * height; i++) {
        unsigned char * c = &image[i*4];
        if (c[0] != transparent.r || c[1] != transparent.g ||
            c[2] != transparent.b)
            continue;
        c[3] = 0;
    }
}

static Image * internal_images[IMAGE_COUNT];

typedef hash_map<std::string, FileImage*> ImageCache;
static ImageCache image_cache;

Image * get_internal_image(unsigned int i)
{
    if (internal_images[i] == NULL) {
        internal_images[i] = new Image(i);
        internal_images[i]->flags |= Image::CACHED;
    }

    Image * image = internal_images[i];
    image->load();

    return internal_images[i];
}

Image * get_image_cache(const std::string & filename, int hot_x, int hot_y,
                        int act_x, int act_y, TransparentColor color)
{
    FileImage * image;
    ImageCache::const_iterator it = image_cache.find(filename);
    if (it == image_cache.end()) {
        image = new FileImage(filename, 0, 0, 0, 0, color);
        image->load();
        if (image->is_valid())
            image->flags |= Image::USED | Image::CACHED;
        else {
            delete image;
            image = NULL;
        }
        image_cache[filename] = image;
    } else {
        image = it->second;
    }
    return image;
}

void reset_image_cache()
{
#ifdef CHOWDREN_TEXTURE_GC
    for (int i = 0; i < IMAGE_COUNT; i++) {
        Image * image = internal_images[i];
        if (image == NULL)
            continue;
        image->flags &= ~Image::USED;
    }
#endif
}

void flush_image_cache()
{
#ifdef CHOWDREN_TEXTURE_GC
    for (int i = 0; i < IMAGE_COUNT; i++) {
        Image * image = internal_images[i];
        if (image == NULL || image->flags & (Image::USED | Image::STATIC))
            continue;
        image->unload();
    }
#endif
}

void preload_images()
{
#if defined(CHOWDREN_PRELOAD_IMAGES) || defined(CHOWDREN_PRELOAD_ALL)
    AssetFile fp;
    fp.open();
    FileStream stream(fp);
    glc_set_storage(true);
    for (int i = 0; i < IMAGE_COUNT; i++) {
        unsigned short handle = stream.read_uint16();
        Image * image = get_internal_image(handle);
        image->upload_texture();
#ifndef CHOWDREN_PRELOAD_ALL
        if (glc_is_vram_full())
            break;
#endif
        image->set_static();
    }
    glc_set_storage(false);
#endif
}

Image dummy_image;

void ReplacedImages::replace(const Color & from, const Color & to)
{
    if (index >= MAX_COLOR_REPLACE) {
        std::cout << "Max color replacements reached" << std::endl;
        return;
    }
    colors[index++] = Replacement(from, to);
}

Image * ReplacedImages::apply(Image * image, Image * src_image)
{
    int count = index;
    index = 0;

    int hash = 0;
    int hash_index = 0;

    for (int i = 0; i < count; i++) {
        const Color & first = colors[i].first;
        const Color & second = colors[i].second;
        hash += first.r * (++hash_index);
        hash += first.g * (++hash_index);
        hash += first.b * (++hash_index);
        hash += second.r * (++hash_index);
        hash += second.g * (++hash_index);
        hash += second.b * (++hash_index);
    }

    vector<ReplacedImage>::const_iterator it2;
    for (it2 = images.begin(); it2 != images.end(); ++it2) {
        const ReplacedImage & img = *it2;
        if (img.hash == hash && img.src_image == src_image) {
            return img.image;
        }
    }

    Image * new_image = image->copy();
    for (int i = 0; i < count; i++) {
        const Color & first = colors[i].first;
        const Color & second = colors[i].second;
        new_image->replace(first, second);
    }

    images.push_back(ReplacedImage(src_image, new_image, hash));
    new_image->flags |= Image::KEEP;
    return new_image;
}

vector<ReplacedImage> ReplacedImages::images;
