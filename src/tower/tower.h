
// tower.h

#pragma once

#include "animation.h"
#include "base.h"
#include "bullet_manager.h"
#include "tower_type.h"

class Tower
{
public:
    Tower();
    virtual ~Tower() = default;

    void Set_position(const Vector2& position);

    const Vector2& Get_size() const;
    const Vector2& Get_position() const;

    void On_update(double delta_time);
    void On_render(SDL_Renderer* renderer);

protected:
    Vector2 size;

    Animation animation_idle_up;
    Animation animation_idle_down;
    Animation animation_idle_left;
    Animation animation_idle_right;

    Animation animation_fire_up;
    Animation animation_fire_down;
    Animation animation_fire_left;
    Animation animation_fire_right;

    TowerType  tower_type  = TowerType::Archer;
    BulletType bullet_type = BulletType::Arrow;

    double fire_speed = 0;

private:
    Vector2 position;

    Timer timer_fire;
    bool  can_fire = true;

    Facing facing = Facing::Right;

    Animation* animation_current = &animation_idle_right;

private:
    void   set_current_ani_dile();
    void   set_current_ani_fire();
    Enemy* find_target_enemy(); // 找到找到进度最大的敌人
    void   on_fire();
};
