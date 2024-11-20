
// wave_manager.cpp

#include "wave_manager.h"

#include "coin_manager.h"
#include "config_manager.h"
#include "enemy_manager.h"
#include "wave.h"

WaveManager::WaveManager()
{
    static const std::vector<Wave>& wave_list = ConfigManager::Instance().wave_list;

    auto func_start_wave = [&]() {
        is_wave_started = true;
        timer_spawn_enemy.set_wait_time(wave_list[idx_wave].spawn_event_list[0].interval);
        timer_spawn_enemy.restart();
    };

    timer_start_wave.set_one_shot(true);
    timer_start_wave.set_wait_time(wave_list[0].interval);
    timer_start_wave.set_on_timeout(func_start_wave);

    auto func_spawn_enemy = [&]() {
        const std::vector<Wave::SpawnEvent>& spawn_event_list = wave_list[idx_wave].spawn_event_list;

        const Wave::SpawnEvent& spawn_event = spawn_event_list[idx_spawn_event];

        EnemyManager::Instance().Spawn_enemy(spawn_event.enemy_type, spawn_event.spawn_point);

        if(++idx_spawn_event >= spawn_event_list.size())
        {
            is_spawned_last_enemy = true;
            return;
        }
        else // 如果还有敌人，要重置计时器
        {
            timer_spawn_enemy.set_wait_time(spawn_event_list[idx_spawn_event].interval);
            timer_spawn_enemy.restart();
        }
    };

    timer_spawn_enemy.set_one_shot(true);
    timer_spawn_enemy.set_on_timeout(func_spawn_enemy);

    timer_start_wave.restart();
}

void
WaveManager::On_update(double delta_time)
{
    static ConfigManager& config_manager = ConfigManager::Instance();

    if(config_manager.is_game_over)
    {
        return;
    }

    if(is_wave_started)
    {
        timer_spawn_enemy.on_update(delta_time);
    }
    else
    {
        timer_start_wave.on_update(delta_time);
    }

    if(is_spawned_last_enemy && EnemyManager::Instance().Ckeck_cleared())
    {
        // 增加金币
        CoinManager::Instance().Increase_coin(config_manager.wave_list[idx_wave].rawards);

        if(++idx_wave >= config_manager.wave_list.size()) // 如果是最后一波，游戏胜利
        {
            config_manager.is_game_win  = true;
            config_manager.is_game_over = true;
        }
        else
        {
            idx_spawn_event       = 0;
            is_wave_started       = true;
            is_spawned_last_enemy = false;

            const Wave& wave = config_manager.wave_list[idx_wave];
            timer_start_wave.set_wait_time(wave.interval);
            timer_start_wave.restart();
        }
    }
}
