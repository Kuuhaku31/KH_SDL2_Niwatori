
// config_manager.h

#pragma once

#include "manager.h"
#include "map.h"
#include "wave.h"

#include "SDL.h"
#include "cJSON.h"

#include <string>

class ConfigManager : public Manager<ConfigManager>
{
    friend class Manager<ConfigManager>;

public:
    struct BasicTemplate
    {
        std::string window_title = (const char*)u8"窗口标题";

        int window_width  = 1280;
        int window_height = 720;
    };

    struct PlayerTemplate
    {
        double speed                  = 3;
        double normal_attack_interval = 0;
        double normal_attack_damage   = 0;
        double skill_interval         = 10;
        double skill_damage           = 1;
    };

    struct TowerTemplate
    {
        double interval[10]    = { 1 };
        double damage[10]      = { 1 };
        double view_range[10]  = { 5 };
        double cost[10]        = { 50 };
        double upgrade_cost[9] = { 75 };
    };

    struct EnemyTemplate
    {
        double hp                = 100;
        double speed             = 1;
        double damage            = 1;
        double reward_ratio      = 0.5;
        double recover_interval  = 10;
        double recover_range     = 0;
        double recover_intensity = 25;
    };

public:
    Map map;

    std::vector<Wave> wave_list;

    int level_archer = 0;
    int level_axeman = 0;
    int level_gunner = 0;

    bool is_game_win  = false;
    bool is_game_over = false;

    SDL_Rect rect_tile_map = { 0 };

    BasicTemplate basic_template;

    PlayerTemplate player_template;

    TowerTemplate archer_template;
    TowerTemplate axeman_template;
    TowerTemplate gunner_template;

    EnemyTemplate slim_template;
    EnemyTemplate king_slim_template;
    EnemyTemplate skeleton_template;
    EnemyTemplate goblin_template;
    EnemyTemplate goblin_priest_template;

    const double num_initial_hp    = 10;
    const double num_initial_coin  = 100;
    const double num_coin_per_prop = 1;

public:
    bool Load_level_config(const std::string& file_path);
    bool Load_game_config(const char* file_path);

protected:
    ConfigManager()  = default;
    ~ConfigManager() = default;

private:
    void parse_basic_temlpate(BasicTemplate& basic_template, cJSON* json_root);
    void parse_player_template(PlayerTemplate& player_template, cJSON* json_root);
    void parse_number_array(double* array, int max_len, cJSON* json_root);
    void parse_tower_template(TowerTemplate& tower_template, cJSON* json_root);
    void parse_enemy_template(EnemyTemplate& enemy_template, cJSON* json_root);
};
