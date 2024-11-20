
// tower_manager.cpp

#include "tower_manager.h"

#include "config_manager.h"
#include "resources_manager.h"

void
TowerManager::On_update(double delta_time)
{
    for(auto tower : tower_list)
    {
        tower->On_update(delta_time);
    }
}

void
TowerManager::On_render(SDL_Renderer* renderer) const
{
    for(auto tower : tower_list)
    {
        tower->On_render(renderer);
    }
}

double
TowerManager::Get_place_cost(TowerType type) const
{
    static ConfigManager& config_manager = ConfigManager::Instance();

    switch(type)
    {
        case TowerType::Archer: return config_manager.archer_template.cost[config_manager.level_archer];
        case TowerType::Axeman: return config_manager.axeman_template.cost[config_manager.level_axeman];
        case TowerType::Gunner: return config_manager.gunner_template.cost[config_manager.level_gunner];
        default: return 0;
    }
}

double
TowerManager::Get_upgrade_cost(TowerType type) const
{
    static ConfigManager& config_manager = ConfigManager::Instance();

    switch(type)
    {
        case TowerType::Archer: return config_manager.level_archer < ARCHER_MAX_LEVEL ? config_manager.archer_template.upgrade_cost[config_manager.level_archer] : -1;
        case TowerType::Axeman: return config_manager.level_axeman < AXEMAN_MAX_LEVEL ? config_manager.axeman_template.upgrade_cost[config_manager.level_axeman] : -1;
        case TowerType::Gunner: return config_manager.level_gunner < GUNNER_MAX_LEVEL ? config_manager.gunner_template.upgrade_cost[config_manager.level_gunner] : -1;
        default: return 0;
    }
}

double
TowerManager::Get_damage_range(TowerType type) const
{
    static ConfigManager& config_manager = ConfigManager::Instance();

    switch(type)
    {
        case TowerType::Archer: return config_manager.archer_template.view_range[config_manager.level_archer];
        case TowerType::Axeman: return config_manager.axeman_template.view_range[config_manager.level_axeman];
        case TowerType::Gunner: return config_manager.gunner_template.view_range[config_manager.level_gunner];
        default: return 0;
    }
}

void
TowerManager::Place_tower(TowerType type, const SDL_Point& pos)
{
    Tower* tower = nullptr;

    switch(type)
    {
        case TowerType::Archer: tower = new ArcherTower(); break;
        case TowerType::Axeman: tower = new AxemanTower(); break;
        case TowerType::Gunner: tower = new GunnerTower(); break;
        default: return;
    }

    static const SDL_Rect& rect_tile_map = ConfigManager::Instance().rect_tile_map;

    static const ResourcesManager::SoundPool& sound_pool = ResourcesManager::Instance().get_sound_pool();

    static Vector2 position;
    position.vx = rect_tile_map.x + pos.x * SIZE_TILE + (SIZE_TILE >> 1);
    position.vy = rect_tile_map.y + pos.y * SIZE_TILE + (SIZE_TILE >> 1);

    tower->Set_position(position);

    tower_list.push_back(tower);

    ConfigManager::Instance().map.Place_tower(pos); // 在地图上标记，防止重复放置

    Mix_PlayChannel(-1, sound_pool.at(ResID::Sound_PlaceTower), 0);
}

void
TowerManager::Upgrade_tower(TowerType type)
{
    static ConfigManager& config_manager = ConfigManager::Instance();

    static const ResourcesManager::SoundPool& sound_pool = ResourcesManager::Instance().get_sound_pool();

    switch(type)
    {
        case TowerType::Archer: config_manager.level_archer = config_manager.level_archer >= ARCHER_MAX_LEVEL ? ARCHER_MAX_LEVEL : config_manager.level_archer + 1; break;
        case TowerType::Axeman: config_manager.level_axeman = config_manager.level_axeman >= AXEMAN_MAX_LEVEL ? AXEMAN_MAX_LEVEL : config_manager.level_axeman + 1; break;
        case TowerType::Gunner: config_manager.level_gunner = config_manager.level_gunner >= GUNNER_MAX_LEVEL ? GUNNER_MAX_LEVEL : config_manager.level_gunner + 1; break;
        default: break;
    }

    Mix_PlayChannel(-1, sound_pool.at(ResID::Sound_TowerLevelUp), 0);
}
