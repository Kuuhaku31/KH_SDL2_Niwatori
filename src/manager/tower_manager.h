
// tower_manager.h

#pragma once

#include "manager.h"
#include "tower.h"

#include <vector>

#define ARCHER_MAX_LEVEL 9
#define AXEMAN_MAX_LEVEL 9
#define GUNNER_MAX_LEVEL 9

class TowerManager : public Manager<TowerManager>
{
    friend class Manager<TowerManager>;

public:
    typedef std::vector<Tower*> TowerList;

public:
    void On_update(double delta_time);
    void On_render(SDL_Renderer* renderer) const;

    double Get_place_cost(TowerType type) const;
    double Get_upgrade_cost(TowerType type) const;
    double Get_damage_range(TowerType type) const;

    void Place_tower(TowerType type, const SDL_Point& pos);
    void Upgrade_tower(TowerType type);

protected:
    TowerManager()  = default;
    ~TowerManager() = default;

private:
    TowerList tower_list;
};
