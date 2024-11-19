
// enemy_manager.h

#pragma once

#include "enemy.h"
#include "manager.h"

#include "SDL.h"

#include <vector>

class EnemyManager : public Manager<EnemyManager>
{
    friend class Manager<EnemyManager>;

public:
    typedef std::vector<Enemy*> EnemyList;

public:
    void On_update(double delta_time);
    void On_render(SDL_Renderer* renderer);

protected:
    EnemyManager() = default;
    ~EnemyManager();

private:
    EnemyList enemy_list;

private:
    void process_home_collision();
    void process_bullet_collision();
    void remove_invalid_enemy();
};
