#ifndef CHOWDREN_COLLISION_H
#define CHOWDREN_COLLISION_H

#include "frame.h"
#include <algorithm>
#include "mathcommon.h"
#include "broadphase.h"
#include "image.h"

bool collide(CollisionBase * a, CollisionBase * b);

enum CollisionType
{
    NONE_COLLISION,
    INSTANCE_BOX,
    BACKDROP_COLLISION,
    SPRITE_COLLISION,
    BOUNDING_BOX,
    BACKGROUND_ITEM
};

enum CollisionFlags
{
    BOX_COLLISION = 1 << 0,
    LADDER_OBSTACLE = 1 << 1,
    HAS_TRANSFORM = 1 << 2
};

class CollisionBase
{
public:
    int aabb[4];
    int flags;
    CollisionType type;

    CollisionBase(CollisionType type, int flags)
    : flags(flags), type(type)
    {
    }

    virtual ~CollisionBase()
    {
    }
};

inline bool collide_line(int x1, int y1, int x2, int y2,
                         int line_x1, int line_y1, int line_x2, int line_y2)
{
    float delta;
    if (line_x2 - line_x1 > line_y2 - line_y1) {
        delta = float(line_y2 - line_y1) / (line_x2 - line_x1);
        if (line_x2 > line_x1) {
            if (x2 < line_x1 || x1 >= line_x2)
                return false;
        } else {
            if (x2 < line_x2 || x1 >= line_x1)
                return false;
        }
        int y = int(delta * (x1 - line_x1) + line_y1);
        if (y >= y1 && y < y2)
            return true;
        y = int(delta * (x2 - line_x1) + line_y1);
        if (y >= y1 && y < y2)
            return true;
        return false;
    } else {
        delta = float(line_x2 - line_x1) / (line_y2 - line_y1);
        if (line_y2 > line_y1) {
            if (y2 < line_y1 || y2 >= line_y2)
                return false;
        } else {
            if (y2 < line_y2 || y1 >= line_y1)
                return false;
        }
        int x = int(delta * (y1 - line_y1) + x1);
        if (x >= x1 && x < x2)
            return true;
        x = int(delta * (y2 - line_y1) + x1);
        if (x >= x1 && x < x2)
            return true;
        return false;
    }
}

class InstanceCollision : public CollisionBase
{
public:
    FrameObject * instance;
    int proxy;

    InstanceCollision(FrameObject * instance, CollisionType type, int flags)
    : instance(instance), CollisionBase(type, flags), proxy(-1)
    {
    }

    ~InstanceCollision()
    {
        remove_proxy();
    }

    void remove_proxy()
    {
        if (proxy == -1)
            return;
        instance->layer->broadphase.remove(proxy);
        proxy = -1;
    }

    void create_proxy()
    {
        if (proxy != -1)
            return;
        proxy = instance->layer->broadphase.add(instance, aabb);
    }

    void create_static_proxy()
    {
        if (proxy != -1)
            return;
        proxy = instance->layer->broadphase.add_static(instance, aabb);
    }

    void update_proxy()
    {
        instance->flags &= ~(HAS_COLLISION_CACHE | HAS_COLLISION);
        if (proxy == -1)
            return;
        instance->layer->broadphase.move(proxy, aabb);
    }

    virtual void update_aabb()
    {
    }
};

class InstanceBox : public InstanceCollision
{
public:
    InstanceBox(FrameObject * instance)
    : InstanceCollision(instance, INSTANCE_BOX, BOX_COLLISION)
    {
    }

    void update_aabb()
    {
        aabb[0] = instance->x;
        aabb[1] = instance->y;
        aabb[2] = aabb[0] + instance->width;
        aabb[3] = aabb[1] + instance->height;
        update_proxy();
    }
};

class OffsetInstanceBox : public InstanceCollision
{
public:
    int off_x, off_y;

    OffsetInstanceBox(FrameObject * instance)
    : InstanceCollision(instance, INSTANCE_BOX, BOX_COLLISION),
      off_x(0), off_y(0)
    {
    }

    void update_aabb()
    {
        aabb[0] = instance->x + off_x;
        aabb[1] = instance->y + off_y;
        aabb[2] = aabb[0] + instance->width;
        aabb[3] = aabb[1] + instance->height;
        update_proxy();
    }

    void set_offset(int x, int y)
    {
        off_x = x;
        off_y = y;
        update_aabb();
    }
};

inline void transform_rect(float xx, float yy, float co, float si,
                           int & x1, int & y1, int & x2, int & y2)
{
    if (co >= 0.0f) {
        if (si >= 0.0f) {
            x1 = 0;
            y1 = int(xx * -si);
            x2 = int(xx * co + yy * si);
            y2 = int(yy * co);
        } else {
            x1 = int(yy * si);
            y1 = 0;
            x2 = int(xx * co);
            y2 = int(yy * co - xx * si);
        }
    } else {
        if (si >= 0.0f) {
            x1 = int(xx * co);
            y1 = int(yy * co - xx * si);
            x2 = int(yy * si);
            y2 = 0;
        } else {
            x1 = int(xx * co + yy * si);
            y1 = int(yy * co);
            x2 = 0;
            y2 = int(-xx * si);
        }
    }
}

#define INTEGER_GET_BIT

#ifdef INTEGER_GET_BIT
#define CONVERT_SCALER(x) (int((x) * 0x7FFF))
#define GET_SCALER_RESULT(x) ((x) >> 15)
#else
#define CONVERT_SCALER(x) x
#define GET_SCALER_RESULT(x) (int(x))
#endif

class SpriteCollision : public InstanceCollision
{
public:
    Image * image;
    float angle;
    float x_scale, y_scale;
    int hotspot_x, hotspot_y;
    // transformed variables
    float co, si;
#ifdef INTEGER_GET_BIT
    int co_divx, si_divx, co_divy, si_divy;
#else
    float co_divx, si_divx, co_divy, si_divy;
#endif
    int x_t, y_t; // transformed offset
    int width, height;
    int new_hotspot_x, new_hotspot_y;

    SpriteCollision(FrameObject * instance = NULL)
    : InstanceCollision(instance, SPRITE_COLLISION, 0), image(NULL),
      angle(0.0f), x_scale(1.0f), y_scale(1.0f), co(1.0f),
      si(0.0f), hotspot_x(0), hotspot_y(0), width(0), height(0), x_t(0), y_t(0)
    {
    }

    void set_hotspot(int x, int y)
    {
        hotspot_x = x;
        hotspot_y = y;
        update_transform();
    }

    void set_image(Image * image, int hotspot_x, int hotspot_y)
    {
        this->image = image;
        this->hotspot_x = hotspot_x;
        this->hotspot_y = hotspot_y;
        update_transform();
    }

    void set_angle(float value)
    {
        angle = value;
        float r = rad(angle);
        co = cos(r);
        si = sin(r);
        update_transform();
    }

    void set_scale(float value)
    {
        x_scale = y_scale = value;
        update_transform();
    }

    void set_x_scale(float x)
    {
        x_scale = x;
        update_transform();
    }

    void set_y_scale(float y)
    {
        y_scale = y;
        update_transform();
    }

    void update_transform()
    {
        bool no_scale = x_scale == 1.0f && y_scale == 1.0f;
        bool no_rotate = angle == 0.0f;
        if (no_scale && no_rotate) {
            width = image->width;
            height = image->height;
            new_hotspot_x = hotspot_x;
            new_hotspot_y = hotspot_y;
            x_t = y_t = 0;
            flags &= ~HAS_TRANSFORM;
            update_aabb();
            return;
        }

        flags |= HAS_TRANSFORM;

        float xx = image->width * x_scale;
        float yy = image->height * y_scale;
        float x_scale_inv = 1.0f / x_scale;
        float y_scale_inv = 1.0f / y_scale;

        if (no_rotate) {
            co_divx = CONVERT_SCALER(x_scale_inv);
            co_divy = CONVERT_SCALER(y_scale_inv);
            si_divx = si_divy = 0.0f;
            width = int(xx);
            height = int(yy);
            x_t = y_t = 0;
            new_hotspot_x = int(hotspot_x * x_scale);
            new_hotspot_y = int(hotspot_y * y_scale);
            update_aabb();
            return;
        }

        co_divx = CONVERT_SCALER(co * x_scale_inv);
        co_divy = CONVERT_SCALER(co * y_scale_inv);
        si_divx = CONVERT_SCALER(si * x_scale_inv);
        si_divy = CONVERT_SCALER(si * y_scale_inv);

        int x2, y2;
        transform_rect(xx, yy, co, si, x_t, y_t, x2, y2);
        width = x2 - x_t;
        height = y2 - y_t;
        get_transform(hotspot_x, hotspot_y,
                      new_hotspot_x, new_hotspot_y);
        update_aabb();
    }

    void get_transform(int x, int y, int & r_x, int & r_y)
    {
        if (!(flags & HAS_TRANSFORM)) {
            r_x = x;
            r_y = y;
            return;
        }
        float xx = x * x_scale;
        float yy = y * y_scale;
        int new_x = int(xx * co + yy * si);
        int new_y = int(yy * co - xx * si);
        r_x = new_x - x_t;
        r_y = new_y - y_t;
    }

    inline bool get_bit(int x, int y)
    {
        // XXX bad branching
        if (flags & HAS_TRANSFORM) {
            int xx = GET_SCALER_RESULT(x * co_divx - y * si_divx);
            int yy = GET_SCALER_RESULT(y * co_divy + x * si_divy);
            x = xx;
            y = yy;
            if (x < 0 || x >= image->width || y < 0 || y >= image->height)
                return false;
        }
        if (flags & BOX_COLLISION)
            return true;
        return image->get_alpha(x, y);
    }

    void update_aabb()
    {
        aabb[0] = instance->x - new_hotspot_x;
        aabb[1] = instance->y - new_hotspot_y;
        aabb[2] = aabb[0] + width;
        aabb[3] = aabb[1] + height;
        update_proxy();
    }
};

class BackdropCollision : public InstanceCollision
{
public:
    Image * image;

    BackdropCollision(FrameObject * instance, Image * image)
    : InstanceCollision(instance, BACKDROP_COLLISION, 0), image(image)
    {
        this->image = image;
    }

    void update_aabb()
    {
        aabb[0] = instance->x;
        aabb[1] = instance->y;
        aabb[2] = aabb[0] + image->width;
        aabb[3] = aabb[1] + image->height;
        update_proxy();
    }

    inline bool get_bit(int x, int y)
    {
        return image->get_alpha(x, y);
    }
};

class PointCollision : public CollisionBase
{
public:
    PointCollision(int x, int y)
    : CollisionBase(BOUNDING_BOX, BOX_COLLISION)
    {
        aabb[0] = x;
        aabb[1] = y;
        aabb[2] = x + 1;
        aabb[3] = y + 1;
    }
};

class BoundingBox : public CollisionBase
{
public:
    BoundingBox(int x1, int y1, int x2, int y2)
    : CollisionBase(BOUNDING_BOX, BOX_COLLISION)
    {
        aabb[0] = x1;
        aabb[1] = y1;
        aabb[2] = x2;
        aabb[3] = y2;
    }
};

class BackgroundItem : public CollisionBase
{
public:
    int dest_x, dest_y, src_x, src_y, src_width, src_height;
    Color color;
    Image * image;

    BackgroundItem(Image * img, int dest_x, int dest_y, int src_x, int src_y,
                   int src_width, int src_height, const Color & color)
    : dest_x(dest_x), dest_y(dest_y), src_x(src_x), src_y(src_y),
      src_width(src_width), src_height(src_height), image(img), color(color),
      CollisionBase(BACKGROUND_ITEM, 0)
    {
        aabb[0] = dest_x;
        aabb[1] = dest_y;
        aabb[2] = dest_x + src_width;
        aabb[3] = dest_y + src_height;
    }

    inline bool get_bit(int x, int y)
    {
        return image->get_alpha(x, y);
    }

    void draw()
    {
        image->draw(dest_x, dest_y, src_x, src_y, src_width, src_height, color);
    }
};

inline bool collide_sprite_background(CollisionBase * a, CollisionBase * b,
                                      int w, int h, int offx1, int offy1,
                                      int offx2, int offy2)
{
    offx1 += ((SpriteCollision*)a)->x_t;
    offy1 += ((SpriteCollision*)a)->y_t;
    offx2 += ((BackgroundItem*)b)->src_x;
    offy2 += ((BackgroundItem*)b)->src_y;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            bool c1 = ((SpriteCollision*)a)->get_bit(offx1 + x, offy1 + y);
            bool c2 = ((BackgroundItem*)b)->get_bit(offx2 + x, offy2 + y);
            if (c1 && c2)
                return true;
        }
    }
    return false;
}

inline bool collide_background_box(CollisionBase * a, int w, int h,
                                   int offx, int offy)
{
    offx += ((BackgroundItem*)a)->src_x;
    offy += ((BackgroundItem*)a)->src_y;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (((BackgroundItem*)a)->get_bit(offx + x, offy + y))
                return true;
        }
    }
    return false;
}

inline bool collide_backdrop_box(CollisionBase * a, int w, int h,
                                 int offx, int offy)
{
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (((BackdropCollision*)a)->get_bit(offx + x, offy + y))
                return true;
        }
    }
    return false;
}

inline bool collide_sprite_box(CollisionBase * a, int w, int h,
                               int offx, int offy)
{
    offx += ((SpriteCollision*)a)->x_t;
    offy += ((SpriteCollision*)a)->y_t;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (((SpriteCollision*)a)->get_bit(offx + x, offy + y))
                return true;
        }
    }
    return false;
}

inline bool collide_sprite_sprite(CollisionBase * a, CollisionBase * b,
                                  int w, int h, int offx1, int offy1,
                                  int offx2, int offy2)
{
    offx1 += ((SpriteCollision*)a)->x_t;
    offy1 += ((SpriteCollision*)a)->y_t;
    offx2 += ((SpriteCollision*)b)->x_t;
    offy2 += ((SpriteCollision*)b)->y_t;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            bool c1 = ((SpriteCollision*)a)->get_bit(offx1 + x, offy1 + y);
            bool c2 = ((SpriteCollision*)b)->get_bit(offx2 + x, offy2 + y);
            if (c1 && c2)
                return true;
        }
    }
    return false;
}

inline bool collide_sprite_backdrop(CollisionBase * a, CollisionBase * b,
                                    int w, int h, int offx1, int offy1,
                                    int offx2, int offy2)
{
    offx1 += ((SpriteCollision*)a)->x_t;
    offy1 += ((SpriteCollision*)a)->y_t;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            bool c1 = ((SpriteCollision*)a)->get_bit(offx1 + x, offy1 + y);
            bool c2 = ((BackdropCollision*)b)->get_bit(offx2 + x, offy2 + y);
            if (c1 && c2)
                return true;
        }
    }
    return false;
}

inline bool collide_direct(CollisionBase * a, CollisionBase * b, int * aabb_2)
{
    int * aabb_1 = a->aabb;
    if (!collides(aabb_1, aabb_2))
        return false;

    if ((a->flags & BOX_COLLISION) && (b->flags & BOX_COLLISION))
        return true;

    // calculate the overlapping area
    int x1, y1, x2, y2;
    intersect(aabb_1[0], aabb_1[1], aabb_1[2], aabb_1[3],
              aabb_2[0], aabb_2[1], aabb_2[2], aabb_2[3],
              x1, y1, x2, y2);

    // figure out the offsets of the overlapping area in each
    int offx1 = x1 - aabb_1[0];
    int offy1 = y1 - aabb_1[1];
    int offx2 = x1 - aabb_2[0];
    int offy2 = y1 - aabb_2[1];

    int w = x2 - x1;
    int h = y2 - y1;

    switch (a->type) {
        case BACKDROP_COLLISION:
            switch (b->type) {
                case SPRITE_COLLISION:
                    return collide_sprite_backdrop(b, a, w, h, offx2, offy2,
                                                   offx1, offy1);
                default:
                    return collide_backdrop_box(a, w, h, offx1, offy1);
            }
        case SPRITE_COLLISION:
            switch (b->type) {
                case SPRITE_COLLISION:
                    return collide_sprite_sprite(a, b, w, h, offx1, offy1,
                                                 offx2, offy2);
                case BACKDROP_COLLISION:
                    return collide_sprite_backdrop(a, b, w, h, offx1, offy1,
                                                   offx2, offy2);
                case BACKGROUND_ITEM:
                    return collide_sprite_background(a, b, w, h, offx1, offy1,
                                                     offx2, offy2);
                default:
                    return collide_sprite_box(a, w, h, offx1, offy1);
            }
        case BACKGROUND_ITEM:
            switch (b->type) {
                case SPRITE_COLLISION:
                    return collide_sprite_background(b, a, w, h, offx2, offy2,
                                                     offx1, offy1);
                default:
                    return collide_background_box(a, w, h, offx1, offy1);
            }
        default:
            // case box
            switch (b->type) {
                case SPRITE_COLLISION:
                    return collide_sprite_box(b, w, h, offx2, offy2);
                case BACKGROUND_ITEM:
                    return collide_background_box(b, w, h, offx2, offy2);
                case BACKDROP_COLLISION:
                    return collide_backdrop_box(b, w, h, offx2, offy2);
                default:
                    return true;
            }
    }
}

inline bool collide(CollisionBase * a, CollisionBase * b)
{
    return collide_direct(a, b, b->aabb);
}

inline bool collide_box(FrameObject * a, int v[4])
{
    CollisionBase * col = a->collision;
    if (col == NULL) {
        int xx1 = a->x;
        int yy1 = a->y;
        int xx2 = xx1 + a->width;
        int yy2 = yy1 + a->height;
        return collides(xx1, yy1, xx2, yy2, v[0], v[1], v[2], v[3]);
    }
    return collides(col->aabb, v);
}

// #include <boost/container/flat_set.hpp>

// class CollisionPairs
// {
// public:
//     struct CollisionPair
//     {
//         FrameObject * a;
//         FrameObject * b;

//         CollisionPair(FrameObject * a, FrameObject * b)
//         : a(a), b(b)
//         {
//         }
//     };

//     flat_set<CollisionPair> pairs;

//     CollisionPairs()
//     {
//     }

//     bool add_pair(FrameObject * a, FrameObject * b)
//     {
//         // returns true if insertion took place
//         return pairs.emplace(a, b).second;
//     }

//     void remove_pair(FrameObject * a, FrameObject * b)
//     {
//         pairs.erase(CollisionPair(a, b));
//     }

//     void update_pairs()
//     {
//     }
// };

#endif // CHOWDREN_COLLISION_H
