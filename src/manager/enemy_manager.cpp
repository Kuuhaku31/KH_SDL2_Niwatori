
// enemy_manager.cpp

#include "config_manager.h"
#include "enemy.h"
#include "enemy_manager.h"
#include "home_manager.h"

#include <algorithm>

EnemyManager::~EnemyManager()
{
    for(Enemy* enemy : enemy_list)
    {
        delete enemy;
    }
}

void
EnemyManager::On_update(double delta_time)
{
    for(Enemy* enemy : enemy_list)
    {
        enemy->On_update(delta_time);
    }
}

void
EnemyManager::On_render(SDL_Renderer* renderer)
{
    for(Enemy* enemy : enemy_list)
    {
        enemy->On_render(renderer);
    }
}

void
EnemyManager::Spawn_enemy(EnemyType enemy_type, int idx_spawn_point)
{
    static const SDL_Rect&           rect_tile_map   = ConfigManager::Instance().rect_tile_map;
    static const Map::SpawnRouteMap& spawn_route_map = ConfigManager::Instance().map.Get_spawn_route_map();

    static Vector2 position_spawn;

    const auto& itor = spawn_route_map.find(idx_spawn_point);
    if(itor == spawn_route_map.end()) return;

    Enemy* enemy = nullptr;

    switch(enemy_type)
    {
        case EnemyType::Slim:
        {
            enemy = new SlimEnemy();
        }
        break;

        case EnemyType::KingSlime:
        {
            enemy = new KingSlimeEnemy();
        }
        break;

        case EnemyType::Goblin:
        {
            enemy = new GoblinEnemy();
        }
        break;

        case EnemyType::GoblinPriest:
        {
            enemy = new GoblinPriestEnemy();
        }
        break;

        case EnemyType::Skeleton:
        {
            enemy = new SkeletonEnemy();
        }
        break;

        default:
        {
            return;
        }
    }

    auto func_on_skill_released = [&](Enemy* enemy_src) {
        double recover_raduis = enemy_src->Get_recover_radius();
        if(recover_raduis < 0) return;

        const Vector2 pos_src = enemy_src->Get_position();
        for(Enemy* enemy_dst : enemy_list) // 遍历所有敌人
        {
            const Vector2 pos_dst = enemy_dst->Get_position();

            double distance = (pos_dst - pos_src).module();
            if(distance <= recover_raduis) // 如果距离小于恢复半径
            {
                enemy_dst->Increase_hp(enemy_src->Get_recover_intensity());
            }
        }
    };

    enemy->Set_on_skill_released(func_on_skill_released);

    const Route::IdxList& idx_list = itor->second.Get_idx_list();

    position_spawn.vx = (double)(rect_tile_map.x + idx_list[0].x * SIZE_TILE + (SIZE_TILE >> 1));
    position_spawn.vy = (double)(rect_tile_map.y + idx_list[0].y * SIZE_TILE + (SIZE_TILE >> 1));

    enemy->Set_position(position_spawn);
    enemy->Set_route(&itor->second);

    enemy_list.push_back(enemy);
}

bool
EnemyManager::Ckeck_cleared() const
{
    return enemy_list.empty();
}

void
EnemyManager::process_home_collision()
{
    static const SDL_Point& idx_home      = ConfigManager::Instance().map.Get_idx_home();
    static const SDL_Rect&  rect_tile_map = ConfigManager::Instance().rect_tile_map;

    static const Vector2 position_home_tile = {
        (double)(rect_tile_map.x + idx_home.x * SIZE_TILE),
        (double)(rect_tile_map.y + idx_home.y * SIZE_TILE)
    };

    for(Enemy* enemy : enemy_list)
    {
        if(enemy->Can_remove()) continue;

        const Vector2& position = enemy->Get_position();

        // 判断是否碰撞到家
        if(position.vx >= position_home_tile.vx && position.vy >= position_home_tile.vy && position.vx <= position_home_tile.vx + SIZE_TILE && position.vy <= position_home_tile.vy + SIZE_TILE)
        {
            enemy->Make_invalid(); // 使敌人无效

            HomeManager::Instance().Decrease_hp(enemy->Get_damage());
        }
    }
}

void
EnemyManager::process_bullet_collision()
{
}

void
EnemyManager::remove_invalid_enemy()
{
    auto f = [](const Enemy* enemy) {
        bool deletable = enemy->Can_remove();
        if(deletable)
        {
            delete enemy;
        }
        return deletable;
    };

    // 删除无效的敌人
    enemy_list.erase(std::remove_if(enemy_list.begin(), enemy_list.end(), f), enemy_list.end());
}
