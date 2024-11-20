
// bullet.cpp

#include "bullet.h"

#include "config_manager.h"

#include <cmath>

void
Bullet::Set_velocity(const Vector2& velocity)
{
    this->velocity = velocity;

    // 如果可以旋转，设置旋转角度
    if(can_rotated)
    {
        angle_animation_rotated = std::atan2(velocity.vy, velocity.vx) * 180 / M_PI;
    }
}

void
Bullet::Set_position(const Vector2& position)
{
    this->position = position;
}

void
Bullet::Set_damage(double damage)
{
    this->damage = damage;
}

void
Bullet::Make_disable_collide()
{
    is_collisional = false;
}

void
Bullet::Make_invalid()
{
    is_valid       = false;
    is_collisional = false;
}

const Vector2&
Bullet::Get_size() const
{
    return size;
}

const Vector2&
Bullet::Get_position() const
{
    return position;
}

const Vector2&
Bullet::Get_velocity() const
{
    return velocity;
}

double
Bullet::Get_damage() const
{
    return damage;
}

double
Bullet::Get_damage_range() const
{
    return damage_range;
}

bool
Bullet::Can_collide() const
{
    return is_collisional;
}

bool
Bullet::Can_remove() const
{
    return !is_valid;
}

void
Bullet::On_update(double delta_time)
{
    static const SDL_Rect& rect_tile_map = ConfigManager::Instance().rect_tile_map;

    animation.on_update(delta_time);
    position += velocity * delta_time;

    // 如果超出屏幕，使其无效
    if(position.vx - size.vx / 2 <= rect_tile_map.x ||
       position.vx + size.vx / 2 >= rect_tile_map.x + rect_tile_map.w ||
       position.vy - size.vy / 2 <= rect_tile_map.y ||
       position.vy + size.vy / 2 >= rect_tile_map.y + rect_tile_map.h)
    {
        is_valid = false;
    }
}

void
Bullet::On_render(SDL_Renderer* renderer)
{
    static SDL_Point pos_display;

    // 设置显示位置
    pos_display.x = (int)(position.vx - size.vx / 2);
    pos_display.y = (int)(position.vy - size.vy / 2);

    // 如果可以旋转，设置旋转角度
    if(can_rotated)
    {
        animation.on_render(renderer, pos_display, angle_animation_rotated);
    }
    else
    {
        animation.on_render(renderer, pos_display);
    }
}

void
Bullet::On_collide(Enemy* enemy)
{
    is_valid       = false;
    is_collisional = false;
}
