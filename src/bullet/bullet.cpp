
// bullet.cpp

#include "bullet.h"

#include "config_manager.h"
#include "resources_manager.h"

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
    if(position.vx + size.vx / 2 <= rect_tile_map.x ||
       position.vx - size.vx / 2 >= rect_tile_map.x + rect_tile_map.w ||
       position.vy - size.vy / 2 >= rect_tile_map.y + rect_tile_map.h ||
       position.vy + size.vy / 2 <= rect_tile_map.y)
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

#define ANIMATION_FRAME_INTERVAL 0.1

// ArrowBullet
#define ARROW_ANIMATION_IDX_LIST { 0, 1 }
#define ARROW_TEXTURE_X_Y 2, 1
#define ARROW_SIZE { 48, 48 }

ArrowBullet::ArrowBullet()
{
    static SDL_Texture* tex_arrow = ResourcesManager::Instance().get_texture_pool().find(ResID::Tex_BulletArrow)->second;

    static const std::vector<int> idx_list = ARROW_ANIMATION_IDX_LIST;

    animation.set_loop(true);
    animation.set_interval(ANIMATION_FRAME_INTERVAL);
    animation.add_frame(tex_arrow, ARROW_TEXTURE_X_Y, idx_list);

    can_rotated = true;

    size = ARROW_SIZE;
}

void
ArrowBullet::On_collide(Enemy* enemy)
{
    static const ResourcesManager::SoundPool& sound_pool = ResourcesManager::Instance().get_sound_pool();

    switch(rand() % 3)
    {
        case 0: Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_ArrowHit_1)->second, 0); break;
        case 1: Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_ArrowHit_2)->second, 0); break;
        case 2: Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_ArrowHit_3)->second, 0); break;
        default: break;
    }

    Bullet::On_collide(enemy);
}


//  AxeBullet
#define AXE_ANIMATION_IDX_LIST { 0, 1, 2, 3, 4, 5, 6, 7 }
#define AXE_TEXTURE_X_Y 4, 2
#define AXE_SIZE { 48, 48 }

AxeBullet::AxeBullet()
{
    static SDL_Texture* tex_axe = ResourcesManager::Instance().get_texture_pool().find(ResID::Tex_BulletAxe)->second;

    static const std::vector<int> idx_list = AXE_ANIMATION_IDX_LIST;

    animation.set_loop(true);
    animation.set_interval(ANIMATION_FRAME_INTERVAL);
    animation.add_frame(tex_axe, AXE_TEXTURE_X_Y, idx_list);

    can_rotated = false;

    size = AXE_SIZE;
}

void
AxeBullet::On_collide(Enemy* enemy)
{
    static const ResourcesManager::SoundPool& sound_pool = ResourcesManager::Instance().get_sound_pool();

    switch(rand() % 3)
    {
        case 0: Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_AxeHit_1)->second, 0); break;
        case 1: Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_AxeHit_2)->second, 0); break;
        case 2: Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_AxeHit_3)->second, 0); break;
        default: break;
    }

    // 减速敌人
    enemy->Slow_down();

    Bullet::On_collide(enemy);
}


// ShellBullet
#define SHELL_ANIMATION_IDX_LIST { 0, 1 }
#define SHELL_TEXTURE_X_Y 2, 1
#define SHELL_SIZE { 48, 48 }

#define EXPLODE_ANIMATION_IDX_LIST { 0, 1, 2, 3, 4 }
#define EXPLODE_TEXTURE_X_Y 5, 1
#define EXPLODE_RANGE 96

ShellBullet::ShellBullet()
{
    static SDL_Texture* tex_shell   = ResourcesManager::Instance().get_texture_pool().find(ResID::Tex_BulletShell)->second;
    static SDL_Texture* tex_explode = ResourcesManager::Instance().get_texture_pool().find(ResID::Tex_EffectExplode)->second;

    static const std::vector<int> idx_list         = SHELL_ANIMATION_IDX_LIST;
    static const std::vector<int> idx_list_explode = EXPLODE_ANIMATION_IDX_LIST;

    animation.set_loop(true);
    animation.set_interval(ANIMATION_FRAME_INTERVAL);
    animation.add_frame(tex_shell, SHELL_TEXTURE_X_Y, idx_list);

    animation_explosion.set_loop(false);
    animation_explosion.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_explosion.add_frame(tex_explode, EXPLODE_TEXTURE_X_Y, idx_list_explode);
    animation_explosion.set_on_finished([this]() { this->Make_invalid(); }); // 爆炸动画结束后使其无效

    can_rotated  = false;
    damage_range = EXPLODE_RANGE;

    size = SHELL_SIZE;
}

void
ShellBullet::On_update(double delta_time)
{
    if(Can_collide())
    {
        Bullet::On_update(delta_time);
        return;
    }
    else
    {
        animation_explosion.on_update(delta_time);
    }
}

void
ShellBullet::On_render(SDL_Renderer* renderer)
{
    if(Can_collide())
    {
        Bullet::On_render(renderer);
        return;
    }
    else
    {
        static SDL_Point pos_display;

        pos_display.x = (int)(position.vx - EXPLODE_RANGE / 2);
        pos_display.y = (int)(position.vy - EXPLODE_RANGE / 2);

        animation_explosion.on_render(renderer, pos_display);
    }
}

void
ShellBullet::On_collide(Enemy* enemy)
{
    static const ResourcesManager::SoundPool& sound_pool = ResourcesManager::Instance().get_sound_pool();

    Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_ShellHit)->second, 0);

    Make_disable_collide();
}
