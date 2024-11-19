
// enemy.h

#pragma once

#include "animation.h"
#include "base.h"
#include "route.h"

class Enemy
{
public:
    typedef std::function<void(Enemy* enemy)> SkillCallback;

public:
    Enemy();
    ~Enemy() = default;

    void On_update(double delta_time);
    void On_render(SDL_Renderer* renderer);

    bool Can_move() const; // 是否可以移动

    double Get_route_process() const; // 获取路径进度

    void Increase_hp(double val); // 增加生命值
    void Decrease_hp(double val); // 减少生命值

    void Slow_down(); // 减速

    void Set_on_skill_released(SkillCallback on_skill_released);
    void Set_position(const Vector2& position); // 设置位置
    void Set_route(const Route* route);         // 设置路径

    void Make_invalid(); // 使无效

    double         Get_hp() const;
    const Vector2& Get_position() const;
    const Vector2& Get_size() const;
    const Vector2& Get_velocity() const;

    double Get_damage() const;
    double Get_reward_ratio() const;
    double Get_recover_radius() const;
    double Get_recover_intensity() const;

protected:
    Vector2 size;

    Timer timer_skill;

    Animation animation_up;
    Animation animation_down;
    Animation animation_left;
    Animation animation_right;

    Animation animation_up_sketch;
    Animation animation_down_sketch;
    Animation animation_left_sketch;
    Animation animation_right_sketch;

    double hp     = 0;
    double max_hp = 0;

    double speed     = 0;
    double max_speed = 0;

    double damage = 0;

    double reward_ratio = 0; // 奖励概率

    double recover_interval  = 0; // 恢复间隔
    double recover_range     = 0; // 恢复范围
    double recover_intensity = 0; // 恢复强度

private:
    Vector2 position;
    Vector2 velocity;
    Vector2 direction;

    bool is_alive = true;

    Timer timer_sketch; // 轮廓显示计时器
    bool  is_show_sketch = false;

    Animation* animation_current = nullptr;

    SkillCallback on_skill_released;

    Timer timer_restore_speed;

    const Route* route = nullptr;

    int     idx_target = 0;
    Vector2 position_target;

private:
    void refresh_position_target(); // 刷新目标位置
};

class SlimEnemy : public Enemy
{
public:
    SlimEnemy();
    ~SlimEnemy() = default;
};

class KingSlimeEnemy : public Enemy
{
public:
    KingSlimeEnemy();
    ~KingSlimeEnemy() = default;
};

class SkeletonEnemy : public Enemy
{
public:
    SkeletonEnemy();
    ~SkeletonEnemy() = default;
};

class GoblinEnemy : public Enemy
{
public:
    GoblinEnemy();
    ~GoblinEnemy() = default;
};

class GoblinPriestEnemy : public Enemy
{
public:
    GoblinPriestEnemy();
    ~GoblinPriestEnemy() = default;
};
