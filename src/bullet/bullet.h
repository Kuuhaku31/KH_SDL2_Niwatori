
// bullet.h

#pragma once

#include "animation.h"
#include "base.h"
#include "enemy.h"

class Bullet
{
public:
    Bullet()  = default;
    ~Bullet() = default;

    void Set_velocity(const Vector2& velocity); // 设置速度
    void Set_position(const Vector2& position); // 设置位置
    void Set_damage(double damage);             // 设置伤害

    void Make_disable_collide(); // 使无碰撞
    void Make_invalid();         // 使无效

    const Vector2& Get_size() const;
    const Vector2& Get_position() const;
    const Vector2& Get_velocity() const;
    double         Get_damage() const;
    double         Get_damage_range() const;

    bool Can_collide() const; // 是否可以碰撞
    bool Can_remove() const;  // 是否可以移除

    virtual void On_update(double delta_time);
    virtual void On_render(SDL_Renderer* renderer);
    virtual void On_collide(Enemy* enemy);

protected:
    Vector2 size;
    Vector2 position;
    Vector2 velocity;

    bool   can_rotated  = false;
    double damage       = 0;
    double damage_range = -1;

    Animation animation;

private:
    bool   is_valid                = true;
    bool   is_collisional          = true;
    double angle_animation_rotated = 0;
};
