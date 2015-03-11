#include "collision.h"

inline bool collide_sprite_background(CollisionBase * a, CollisionBase * b,
                                      int w, int h, int offx1, int offy1,
                                      int offx2, int offy2)
{
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

inline bool collide_tsprite_background(CollisionBase * a, CollisionBase * b,
                                       int w, int h, int offx1, int offy1,
                                       int offx2, int offy2)
{
    offx1 += ((SpriteCollision*)a)->x_t;
    offy1 += ((SpriteCollision*)a)->y_t;
    offx2 += ((BackgroundItem*)b)->src_x;
    offy2 += ((BackgroundItem*)b)->src_y;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            bool c1 = ((SpriteCollision*)a)->get_tbit(offx1 + x, offy1 + y);
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
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (((SpriteCollision*)a)->get_bit(offx + x, offy + y))
                return true;
        }
    }
    return false;
}

inline bool collide_tsprite_box(CollisionBase * a, int w, int h,
                                int offx, int offy)
{
    offx += ((SpriteCollision*)a)->x_t;
    offy += ((SpriteCollision*)a)->y_t;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (((SpriteCollision*)a)->get_tbit(offx + x, offy + y))
                return true;
        }
    }
    return false;
}

inline bool collide_sprite_sprite(CollisionBase * a, CollisionBase * b,
                                  int w, int h, int offx1, int offy1,
                                  int offx2, int offy2)
{
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

inline bool collide_sprite_tsprite(CollisionBase * a, CollisionBase * b,
                                   int w, int h, int offx1, int offy1,
                                   int offx2, int offy2)
{
    offx2 += ((SpriteCollision*)b)->x_t;
    offy2 += ((SpriteCollision*)b)->y_t;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            bool c1 = ((SpriteCollision*)a)->get_bit(offx1 + x, offy1 + y);
            bool c2 = ((SpriteCollision*)b)->get_tbit(offx2 + x, offy2 + y);
            if (c1 && c2)
                return true;
        }
    }
    return false;
}

inline bool collide_tsprite_tsprite(CollisionBase * a, CollisionBase * b,
                                    int w, int h, int offx1, int offy1,
                                    int offx2, int offy2)
{
    offx1 += ((SpriteCollision*)a)->x_t;
    offy1 += ((SpriteCollision*)a)->y_t;
    offx2 += ((SpriteCollision*)b)->x_t;
    offy2 += ((SpriteCollision*)b)->y_t;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            bool c1 = ((SpriteCollision*)a)->get_tbit(offx1 + x, offy1 + y);
            bool c2 = ((SpriteCollision*)b)->get_tbit(offx2 + x, offy2 + y);
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

inline bool collide_tsprite_backdrop(CollisionBase * a, CollisionBase * b,
                                     int w, int h, int offx1, int offy1,
                                     int offx2, int offy2)
{
    offx1 += ((SpriteCollision*)a)->x_t;
    offy1 += ((SpriteCollision*)a)->y_t;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            bool c1 = ((SpriteCollision*)a)->get_tbit(offx1 + x, offy1 + y);
            bool c2 = ((BackdropCollision*)b)->get_bit(offx2 + x, offy2 + y);
            if (c1 && c2)
                return true;
        }
    }
    return false;
}

bool collide_direct(CollisionBase * a, CollisionBase * b, int * aabb_2)
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
                case TRANSFORM_SPRITE_COLLISION:
                    return collide_tsprite_backdrop(b, a, w, h, offx2, offy2,
                                                    offx1, offy1);
                default:
                    return collide_backdrop_box(a, w, h, offx1, offy1);
            }
        case SPRITE_COLLISION:
            switch (b->type) {
                case SPRITE_COLLISION:
                    return collide_sprite_sprite(a, b, w, h, offx1, offy1,
                                                 offx2, offy2);
                case TRANSFORM_SPRITE_COLLISION:
                    return collide_sprite_tsprite(a, b, w, h, offx1, offy1,
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
        case TRANSFORM_SPRITE_COLLISION:
            switch (b->type) {
                case SPRITE_COLLISION:
                    return collide_sprite_tsprite(b, a, w, h, offx2, offy2,
                                                 offx1, offy1);
                case TRANSFORM_SPRITE_COLLISION:
                    return collide_tsprite_tsprite(a, b, w, h, offx1, offy1,
                                                   offx2, offy2);
                case BACKDROP_COLLISION:
                    return collide_tsprite_backdrop(a, b, w, h, offx1, offy1,
                                                    offx2, offy2);
                case BACKGROUND_ITEM:
                    return collide_tsprite_background(a, b, w, h, offx1, offy1,
                                                      offx2, offy2);
                default:
                    return collide_tsprite_box(a, w, h, offx1, offy1);
            }
        case BACKGROUND_ITEM:
            switch (b->type) {
                case SPRITE_COLLISION:
                    return collide_sprite_background(b, a, w, h, offx2, offy2,
                                                     offx1, offy1);
                case TRANSFORM_SPRITE_COLLISION:
                    return collide_tsprite_background(b, a, w, h, offx2, offy2,
                                                      offx1, offy1);
                default:
                    return collide_background_box(a, w, h, offx1, offy1);
            }
        default:
            // case box
            switch (b->type) {
                case SPRITE_COLLISION:
                    return collide_sprite_box(b, w, h, offx2, offy2);
                case TRANSFORM_SPRITE_COLLISION:
                    return collide_tsprite_box(b, w, h, offx2, offy2);
                case BACKGROUND_ITEM:
                    return collide_background_box(b, w, h, offx2, offy2);
                case BACKDROP_COLLISION:
                    return collide_backdrop_box(b, w, h, offx2, offy2);
                default:
                    return true;
            }
    }
}
