
// tower.cpp

#include "tower.h"

#include "config_manager.h"
#include "enemy_manager.h"
#include "resources_manager.h"

#define ANIMATION_FRIME_INTERVAL 0.2

Tower::Tower()
{
    timer_fire.set_one_shot(true);
    timer_fire.set_on_timeout([this]() { this->can_fire = true; });

    animation_idle_up.set_loop(true);
    animation_idle_up.set_interval(ANIMATION_FRIME_INTERVAL);

    animation_idle_down.set_loop(true);
    animation_idle_down.set_interval(ANIMATION_FRIME_INTERVAL);

    animation_idle_left.set_loop(true);
    animation_idle_left.set_interval(ANIMATION_FRIME_INTERVAL);

    animation_idle_right.set_loop(true);
    animation_idle_right.set_interval(ANIMATION_FRIME_INTERVAL);

    animation_fire_up.set_loop(false);
    animation_fire_up.set_interval(ANIMATION_FRIME_INTERVAL);
    animation_fire_up.set_on_finished([this]() { this->set_current_ani_dile(); });

    animation_fire_down.set_loop(false);
    animation_fire_down.set_interval(ANIMATION_FRIME_INTERVAL);
    animation_fire_down.set_on_finished([this]() { this->set_current_ani_dile(); });

    animation_fire_left.set_loop(false);
    animation_fire_left.set_interval(ANIMATION_FRIME_INTERVAL);
    animation_fire_left.set_on_finished([this]() { this->set_current_ani_dile(); });

    animation_fire_right.set_loop(false);
    animation_fire_right.set_interval(ANIMATION_FRIME_INTERVAL);
    animation_fire_right.set_on_finished([this]() { this->set_current_ani_dile(); });
}

void
Tower::Set_position(const Vector2& position)
{
    this->position = position;
}

const Vector2&
Tower::Get_size() const
{
    return size;
}

const Vector2&
Tower::Get_position() const
{
    return position;
}

void
Tower::On_update(double delta_time)
{
    timer_fire.on_update(delta_time);
    animation_current->on_update(delta_time);

    if(can_fire)
    {
        on_fire();
    }
}

void
Tower::On_render(SDL_Renderer* renderer)
{
    static SDL_Point dst_point;

    dst_point.x = (int)(position.vx - size.vx / 2);
    dst_point.y = (int)(position.vy - size.vy / 2);

    animation_current->on_render(renderer, dst_point);
}


void
Tower::set_current_ani_dile()
{
    switch(facing)
    {
        case Facing::Up: animation_current = &animation_idle_up; break;
        case Facing::Down: animation_current = &animation_idle_down; break;
        case Facing::Left: animation_current = &animation_idle_left; break;
        case Facing::Right: animation_current = &animation_idle_right; break;
        default: break;
    }
}

void
Tower::set_current_ani_fire()
{
    switch(facing)
    {
        case Facing::Up: animation_current = &animation_fire_up; break;
        case Facing::Down: animation_current = &animation_fire_down; break;
        case Facing::Left: animation_current = &animation_fire_left; break;
        case Facing::Right: animation_current = &animation_fire_right; break;
        default: break;
    }
}

Enemy*
Tower::find_target_enemy() // 找到找到进度最大的敌人
{
    static ConfigManager& config_manager = ConfigManager::Instance();

    double process      = -1; // 寻找最大值
    double view_range   = 0;
    Enemy* target_enemy = nullptr;

    switch(tower_type)
    {
        case TowerType::Archer: view_range = config_manager.archer_template.view_range[config_manager.level_archer]; break;
        case TowerType::Axeman: view_range = config_manager.axeman_template.view_range[config_manager.level_axeman]; break;
        case TowerType::Gunner: view_range = config_manager.gunner_template.view_range[config_manager.level_gunner]; break;
        default: break;
    }

    EnemyManager::EnemyList& enemy_list = EnemyManager::Instance().Get_enemy_list();
    for(Enemy* enemy : enemy_list) // 遍历敌人列表，找到进度最大的敌人
    {
        // 如果在距离内
        if((enemy->Get_position() - position).module() <= view_range * SIZE_TILE)
        {
            double new_process = enemy->Get_route_process();
            if(new_process > process)
            {
                process      = new_process;
                target_enemy = enemy;
            }
        }
    }

    return target_enemy;
}

void
Tower::on_fire()
{
    static ConfigManager& config_manager = ConfigManager::Instance();

    static const ResourcesManager::SoundPool& sound_pool = ResourcesManager::Instance().get_sound_pool();

    Enemy* target_enemy = find_target_enemy();
    if(!target_enemy) return;

    can_fire = false;

    double interval = 0;
    double damage   = 0;

    switch(tower_type)
    {
        case TowerType::Archer:
        {
            interval = config_manager.archer_template.interval[config_manager.level_archer];
            damage   = config_manager.archer_template.damage[config_manager.level_archer];
            switch(rand() % 2)
            {
                case 0: Mix_PlayChannel(-1, sound_pool.at(ResID::Sound_ArrowFire_1), 0); break;
                case 1: Mix_PlayChannel(-1, sound_pool.at(ResID::Sound_ArrowFire_2), 0); break;
                default: break;
            }
            break;
        }

        case TowerType::Axeman:
        {
            interval = config_manager.axeman_template.interval[config_manager.level_axeman];
            damage   = config_manager.axeman_template.damage[config_manager.level_axeman];
            Mix_PlayChannel(-1, sound_pool.at(ResID::Sound_AxeFire), 0);
            break;
        }

        case TowerType::Gunner:
        {
            interval = config_manager.gunner_template.interval[config_manager.level_gunner];
            damage   = config_manager.gunner_template.damage[config_manager.level_gunner];
            Mix_PlayChannel(-1, sound_pool.at(ResID::Sound_ShellFire), 0);
            break;
        }

        default: break;
    }

    timer_fire.set_wait_time(interval);
    timer_fire.restart();

    Vector2 direction = (target_enemy->Get_position() - position).unit();

    BulletManager::Instance().Fire_bullet(bullet_type, position, direction * fire_speed * SIZE_TILE, damage);

    if(abs(direction.vx) >= abs(direction.vy)) // 根据方向设置朝向
    {
        facing = direction.vx > 0 ? Facing::Right : Facing::Left;
    }
    else
    {
        facing = direction.vy > 0 ? Facing::Down : Facing::Up;
    }

    set_current_ani_fire();

    animation_current->reset();
}
