
// enemy_manager.cpp

#include "config_manager.h"
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
