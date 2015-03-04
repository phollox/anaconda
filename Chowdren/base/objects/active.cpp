#include "objects/active.h"
#include "manager.h"
#include "render.h"

// Active

Active::Active(int x, int y, int type_id)
: FrameObject(x, y, type_id), forced_animation(-1),
  animation_frame(0), counter(0), angle(0.0f), forced_frame(-1),
  forced_speed(-1), forced_direction(-1), x_scale(1.0f), y_scale(1.0f),
  animation_direction(0), stopped(false), flash_interval(0.0f),
  animation_finished(-1), transparent(false), image(NULL), direction_data(NULL)
{
    sprite_col.instance = this;
    collision = &sprite_col;
}

void Active::initialize_active()
{
    if (collision_box)
        sprite_col.flags |= BOX_COLLISION;
    update_direction();

    int n = 1;
    if (current_animation == APPEARING || current_animation == DISAPPEARING)
        n++;
    counter += int(direction_data->max_speed * manager.frame->timer_mul) * n;
}

Active::~Active()
{
}

void Active::set_animation(int value)
{
    if (value == animation)
        return;
    value = get_animation(value);
    if (value == animation)
        return;
    animation = value;
    if (forced_animation >= 0)
        return;
    animation_frame = 0;
    current_animation = animation;
    update_direction();
}

void Active::force_animation(int value)
{
    if (value == forced_animation)
        return;
    value = get_animation(value);
    if (value == forced_animation)
        return;
    if (flags & FADEOUT) {
        FrameObject::destroy();
        return;
    }
    forced_animation = value;
    if (value == current_animation)
        return;
    animation_frame = 0;
    current_animation = value;
    update_direction();
}

void Active::force_frame(int value)
{
    if (loop_count == 0)
        return;
    int frame_count = direction_data->frame_count;
    forced_frame = int_max(0, int_min(value, frame_count - 1));
    update_frame();
}

void Active::force_speed(int value)
{
    int delta = direction_data->max_speed - direction_data->min_speed;
    if (delta != 0) {
        value = (value * delta) / 100 + direction_data->min_speed;
        value = std::min(direction_data->max_speed, value);
    }
    forced_speed = value;
}

void Active::force_direction(int value)
{
    value &= 31;
    if (forced_direction == value)
        return;
    forced_direction = value;
    update_direction();
}

void Active::restore_direction()
{
    forced_direction = -1;
    update_direction();
}

void Active::restore_animation()
{
    if (forced_animation == -1)
        return;
    forced_animation = -1;
    if (current_animation == animation)
        return;
    current_animation = animation;
    animation_frame = 0;
    update_direction();
}

void Active::restore_frame()
{
    if (forced_frame == -1)
        return;
    animation_frame = forced_frame;
    forced_frame = -1;
    update_frame();
}

void Active::restore_speed()
{
    forced_speed = -1;
}

void Active::update_frame()
{
    Image * new_image = direction_data->frames[get_frame()];
    if (new_image == image)
        return;
    image = new_image;
    image->load();

    sprite_col.set_image(image, image->hotspot_x, image->hotspot_y);
    update_action_point();
}

void Active::update_direction(Direction * dir)
{
    if (dir == NULL)
        dir = get_direction_data();

    direction_data = dir;
    loop_count = direction_data->loop_count;

    // make sure frame is still in range
    int frame_count = direction_data->frame_count;
    if (forced_frame != -1 && forced_frame >= frame_count)
        forced_frame = -1;
    if (animation_frame >= frame_count)
        animation_frame = 0;

    update_frame();
}

void Active::update_action_point()
{
    sprite_col.get_transform(image->action_x, image->action_y,
                             action_x, action_y);
    action_x -= sprite_col.new_hotspot_x;
    action_y -= sprite_col.new_hotspot_y;
}

void Active::update()
{
#ifdef CHOWDREN_DEFER_COLLISIONS
        flags |= DEFER_COLLISIONS;
        memcpy(old_aabb, sprite_col.aabb, sizeof(old_aabb));
#endif
    if (flags & FADEOUT && animation_finished == DISAPPEARING) {
        FrameObject::destroy();
        return;
    }

    update_flash(flash_interval, flash_time);

    animation_finished = -1;

    if (forced_animation == -1 && animation != current_animation) {
        current_animation = animation;
        animation_frame = 0;
        update_direction();
    }

    if (forced_frame != -1 || stopped || loop_count == 0) {
        return;
    }

    counter += int(get_speed() * frame->timer_mul);
    int old_frame = animation_frame;

    while (counter > 100) {
        counter -= 100;
        animation_frame++;
        if (animation_frame < direction_data->frame_count)
            continue;
        if (loop_count > 0)
            loop_count--;
        if (loop_count != 0) {
            animation_frame = direction_data->back_to;
            continue;
        }

        animation_finished = current_animation;
        animation_frame--;

        if (forced_animation != -1) {
            forced_animation = -1;
            forced_speed = -1;
            forced_direction = -1;
        }
        return;
    }
    if (animation_frame != old_frame)
        update_frame();
}

inline int get_active_load_point(int value, int max)
{
    if (value == 100000) {
        return max / 2;
    } else if (value == 110000) {
        return max;
    }
    return value;
}

void Active::load(const std::string & filename, int anim, int dir, int frame,
                  int hot_x, int hot_y, int action_x, int action_y,
                  TransparentColor transparent_color)
{
    if (anim >= animations->count)
        return;
    if (dir < 0 || dir >= 32)
        return;
    Animation * animation = animations->items[anim];
    Direction * direction = animation->dirs[dir];
    if (frame >= direction->frame_count)
        return;

    Image * new_image = get_image_cache(convert_path(filename), 0, 0, 0, 0,
                                        transparent_color);

    if (new_image == NULL) {
        std::cout << "Could not load image " << filename << std::endl;
        return;
    }

    new_image->hotspot_x = get_active_load_point(hot_x, new_image->width);
    new_image->hotspot_y = get_active_load_point(hot_y, new_image->height);
    new_image->action_x = get_active_load_point(action_x, new_image->width);
    new_image->action_y = get_active_load_point(action_y, new_image->height);

    Image * old_image = direction->frames[frame];
    if (old_image == new_image)
        return;

    old_image->destroy();
    new_image->upload_texture();
    direction->frames[frame] = new_image;

    // update the frame for all actives of this type
    // this may break in the future, maybe
    ObjectList::iterator it;
    ObjectList & list = this->frame->instances.items[id];
    for (it = list.begin(); it != list.end(); ++it) {
        Active * obj = (Active*)it->obj;
        obj->image = NULL;
        obj->update_frame();
    }
}

void Active::draw()
{
    bool blend = transparent || blend_color.a < 255 ||
                 effect != Render::NONE;
    if (blend) {
        draw_image(image, x, y, blend_color, angle, x_scale, y_scale);
        return;
    }
    Render::disable_blend();
    draw_image(image, x, y, blend_color, angle, x_scale, y_scale);
    Render::enable_blend();
}

int Active::get_action_x()
{
    return get_x() + action_x;
}

int Active::get_action_y()
{
    return get_y() + action_y;
}

void Active::set_angle(float angle, int quality)
{
    angle = mod(angle, 360.0f);
    this->angle = angle;
    sprite_col.set_angle(angle);
    update_action_point();
}

float Active::get_angle()
{
    return angle;
}

int Active::get_frame()
{
    if (forced_frame != -1)
        return forced_frame;
    return animation_frame;
}

int Active::get_speed()
{
    if (forced_speed != -1)
        return forced_speed;
    return direction_data->max_speed;
}

Direction * Active::get_direction_data()
{
    Animation * anim = animations->items[current_animation];

    if (anim == NULL) {
        std::cout << "Invalid animation: " << current_animation << std::endl;
        return NULL;
    }

    int dir = get_animation_direction();
    Direction * data = anim->dirs[dir];
    if (data != NULL)
        return data;

    int search_dir = 1;
    if (direction_data != NULL && ((dir - direction_data->index) & 31) <= 15)
        search_dir = -1;

    while (true) {
        dir = (dir + search_dir) & 31;
        Direction * new_data = anim->dirs[dir];
        if (new_data)
            return new_data;
    }
    return NULL;
}

static int animation_alias[] = {
    APPEARING, WALKING, RUNNING,
    RUNNING, STOPPED, -1,
    WALKING, STOPPED, -1,
    STOPPED, WALKING, RUNNING,
    STOPPED, -1, -1,
    STOPPED, WALKING, RUNNING,
    STOPPED, WALKING, RUNNING,
    WALKING, RUNNING, STOPPED,
    STOPPED, WALKING, RUNNING,
    WALKING, RUNNING, STOPPED,
    STOPPED, WALKING, RUNNING,
    STOPPED, WALKING, RUNNING,
    -1, -1, -1,
    -1, -1, -1,
    -1, -1, -1,
    -1, -1, -1,
};

int Active::get_animation(int value)
{
    if (has_animation(value))
        return value;
    for (int i = 0; i < 3; i++) {
        int alias = animation_alias[i + value * 3];
        if (alias == -1 || !has_animation(alias))
            break;
        return alias;
    }
    for (value = 0; value < animations->count; value++) {
        if (has_animation(value))
            break;
    }
    return value;
}

void Active::set_direction(int value, bool set_movement)
{
    value &= 31;
    FrameObject::set_direction(value, set_movement);
    if (auto_rotate) {
        set_angle(float(value) * 11.25f);
        value = 0;
    }
    if (value == animation_direction)
        return;
    animation_direction = value;
    Direction * old_dir = direction_data;
    Direction * new_dir = get_direction_data();
    if (old_dir == new_dir)
        return;
    update_direction(new_dir);
}

int Active::get_animation_direction()
{
    if (forced_direction != -1)
        return forced_direction;
    return animation_direction;
}

void Active::set_scale(float value)
{
    value = std::max(0.0f, value);
    x_scale = y_scale = value;
    sprite_col.set_scale(value);
    update_action_point();
}

void Active::set_x_scale(float value)
{
    x_scale = std::max(0.0f, value);
    sprite_col.set_x_scale(x_scale);
    update_action_point();
}

void Active::set_y_scale(float value)
{
    y_scale = std::max(0.0f, value);
    sprite_col.set_y_scale(y_scale);
    update_action_point();
}

void Active::paste(int collision_type)
{
    layer->paste(image, x-image->hotspot_x, y-image->hotspot_y, 0, 0,
                 image->width, image->height, collision_type, blend_color);
}

bool Active::test_animation(int value)
{
    if (value != current_animation)
        return false;
    if (loop_count == 0)
        return false;
    return true;
}

void Active::stop_animation()
{
    stopped = true;
}

void Active::start_animation()
{
    stopped = false;
}

void Active::flash(float value)
{
    flash_interval = value;
    flash_time = 0.0f;
}

bool Active::is_animation_finished(int anim)
{
    return animation_finished == anim;
}

void Active::destroy()
{
    if (flags & FADEOUT)
        return;
    if (!has_animation(DISAPPEARING)) {
        FrameObject::destroy();
        return;
    }
    clear_movements();

    if (forced_animation != DISAPPEARING) {
        restore_animation();
        force_animation(DISAPPEARING);
    }
    flags |= FADEOUT;
    collision->type = NONE_COLLISION;
}

bool Active::has_animation(int anim)
{
    if (anim >= animations->count)
        return false;
    if (animations->items[anim] == NULL)
        return false;
    return true;
}

void Active::replace_color(const Color & from, const Color & to)
{
}

class DefaultActive : public Active
{
public:
    DefaultActive()
    : Active(0, 0, 0)
    {
        setup_default_instance(this);
        collision = new InstanceBox(this);
        create_alterables();
    }
};

static DefaultActive default_active;
FrameObject * default_active_instance = &default_active;
