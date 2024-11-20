
// wave_manager.h

#pragma once

#include "base.h"
#include "manager.h"

class WaveManager : public Manager<WaveManager>
{
    friend class Manager<WaveManager>;

public:
    void On_update(double delta_time);

protected:
    WaveManager();
    ~WaveManager() = default;

private:
    int   idx_wave        = 0;
    int   idx_spawn_event = 0;
    Timer timer_start_wave;
    Timer timer_spawn_enemy;
    bool  is_wave_started       = false;
    bool  is_spawned_last_enemy = false;
};
