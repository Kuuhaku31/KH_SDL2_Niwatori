
// config_manager.h

#pragma once

#include "manager.h"
#include "map.h"
#include "wave.h"

#include "SDL.h"
#include "cJSON.h"

#include <fstream>
#include <iostream>
#include <sstream>
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
    bool
    Load_level_config(const std::string& file_path)
    {
        std::stringstream str_stream;

        std::ifstream file(file_path);
        if(file.good())
        {
            str_stream << file.rdbuf(); // 读取文件内容
            file.close();
        }
        else
        {
            return false;
        }

        cJSON* json_root = cJSON_Parse(str_stream.str().c_str());
        if(!json_root)
        {
            return false;
        }
        else if(json_root->type != cJSON_Array) // 如果不是数组
        {
            cJSON_Delete(json_root); // 释放内存
            return false;
        }

        cJSON* json_wave = nullptr;
        cJSON_ArrayForEach(json_wave, json_root)
        {
            if(json_wave->type != cJSON_Object) // 如果不是对象
            {
                continue;
            }

            wave_list.emplace_back();      // 添加新的波次
            Wave& wave = wave_list.back(); // 获取最后一个波次

            // 获取波次的属性
            cJSON* json_wave_rewards = cJSON_GetObjectItem(json_wave, "rewards");
            if(json_wave_rewards && json_wave_rewards->type == cJSON_Number) // 如果是数字
            {
                wave.rawards = json_wave_rewards->valuedouble;
            }

            // 获取波次的间隔
            cJSON* json_wave_interval = cJSON_GetObjectItem(json_wave, "interval");
            if(json_wave_interval && json_wave_interval->type == cJSON_Number) // 如果是数字
            {
                wave.interval = json_wave_interval->valuedouble;
            }

            // 获取波次的敌人
            cJSON* json_wave_spawn_list = cJSON_GetObjectItem(json_wave, "spawn_list");
            if(json_wave_spawn_list && json_wave_spawn_list->type == cJSON_Array) // 如果是数组
            {
                cJSON* json_spawn_event = nullptr;
                cJSON_ArrayForEach(json_spawn_event, json_wave_spawn_list)
                {
                    if(json_spawn_event->type != cJSON_Object) // 如果不是对象
                    {
                        continue;
                    }

                    wave.spawn_event_list.emplace_back();                         // 添加新的生成事件
                    Wave::SpawnEvent& spawn_event = wave.spawn_event_list.back(); // 获取最后一个生成事件

                    // 获取生成事件的属性
                    cJSON* json_spawn_interval = cJSON_GetObjectItem(json_spawn_event, "interval");
                    if(json_spawn_interval && json_spawn_interval->type == cJSON_Number) // 如果是数字
                    {
                        spawn_event.interval = json_spawn_interval->valuedouble;
                    }

                    cJSON* json_spawn_point = cJSON_GetObjectItem(json_spawn_event, "point");
                    if(json_spawn_point && json_spawn_point->type == cJSON_Number) // 如果是数字
                    {
                        spawn_event.spawn_point = json_spawn_point->valueint;
                    }

                    cJSON* json_spawn_enemy_type = cJSON_GetObjectItem(json_spawn_event, "enemy");
                    if(json_spawn_enemy_type && json_spawn_enemy_type->type == cJSON_String) // 如果是字符串
                    {
                        const std::string str_enemy_type = json_spawn_enemy_type->valuestring;
                        if(str_enemy_type == "slim")
                        {
                            spawn_event.enemy_type = EnemyType::Slim;
                        }
                        else if(str_enemy_type == "king_slime")
                        {
                            spawn_event.enemy_type = EnemyType::KingSlime;
                        }
                        else if(str_enemy_type == "skeleton")
                        {
                            spawn_event.enemy_type = EnemyType::Skeleton;
                        }
                        else if(str_enemy_type == "goblin")
                        {
                            spawn_event.enemy_type = EnemyType::Goblin;
                        }
                        else if(str_enemy_type == "goblin_priest")
                        {
                            spawn_event.enemy_type = EnemyType::GoblinPriest;
                        }
                    }

                    // 检测是否合法
                    if(wave.spawn_event_list.empty())
                    {
                        wave_list.pop_back();
                    }
                }
            }
        }

        cJSON_Delete(json_root); // 释放内存

        // 检查是否为空
        if(wave_list.empty())
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    bool
    Load_game_config(const char* file_path)
    {
        std::stringstream str_stream;

        std::ifstream file(file_path);
        if(file.good())
        {
            str_stream << file.rdbuf(); // 读取文件内容
            file.close();
        }
        else
        {
            return false;
        }

        cJSON* json_root = cJSON_Parse(str_stream.str().c_str()); // 解析JSON
        if(!json_root)
        {
            return false;
        }
        else if(json_root->type != cJSON_Object) // 如果不是对象
        {
            cJSON_Delete(json_root); // 释放内存
            return false;
        }

        // 获取基本模板
        cJSON* json_basic  = cJSON_GetObjectItem(json_root, "basic");
        cJSON* json_player = cJSON_GetObjectItem(json_root, "player");
        cJSON* json_tower  = cJSON_GetObjectItem(json_root, "tower");
        cJSON* json_enemy  = cJSON_GetObjectItem(json_root, "enemy");

        bool all_not_empty    = json_basic && json_player && json_tower && json_enemy;
        bool all_correct_type = json_basic->type == cJSON_Object && json_player->type == cJSON_Object && json_tower->type == cJSON_Object && json_enemy->type == cJSON_Object;
        if(!all_not_empty || !all_correct_type) // 如果为空或者类型不正确
        {
            cJSON_Delete(json_root); // 释放内存
            return false;
        }

        // 解析模板

        parse_basic_temlpate(basic_template, json_basic); // 解析基本模板

        parse_player_template(player_template, json_player); // 解析玩家模板

        parse_tower_template(archer_template, cJSON_GetObjectItem(json_tower, "archer")); // 解析弓箭手模板，需要先获取玩家节点为根节点
        parse_tower_template(axeman_template, cJSON_GetObjectItem(json_tower, "axeman")); // 解析斧手模板
        parse_tower_template(gunner_template, cJSON_GetObjectItem(json_tower, "gunner")); // 解析枪手模板

        parse_enemy_template(slim_template, cJSON_GetObjectItem(json_enemy, "slim"));                   // 解析史莱姆模板
        parse_enemy_template(king_slim_template, cJSON_GetObjectItem(json_enemy, "king_slime"));        // 解析史莱姆王模板
        parse_enemy_template(skeleton_template, cJSON_GetObjectItem(json_enemy, "skeleton"));           // 解析骷髅模板
        parse_enemy_template(goblin_template, cJSON_GetObjectItem(json_enemy, "goblin"));               // 解析哥布林模板
        parse_enemy_template(goblin_priest_template, cJSON_GetObjectItem(json_enemy, "goblin_priest")); // 解析哥布林牧师模板

        cJSON_Delete(json_root); // 释放内存
        return true;
    }

protected:
    ConfigManager()  = default;
    ~ConfigManager() = default;

private:
    void
    parse_basic_temlpate(BasicTemplate& basic_template, cJSON* json_root)
    {
        if(!json_root || json_root->type != cJSON_Object) // 如果为空或者不是对象
        {
            return;
        }

        cJSON* json_window_title = cJSON_GetObjectItem(json_root, "window_title");
        if(json_window_title && json_window_title->type == cJSON_String) // 如果是字符串
        {
            basic_template.window_title = json_window_title->valuestring;
        }

        cJSON* json_window_width = cJSON_GetObjectItem(json_root, "window_width");
        if(json_window_width && json_window_width->type == cJSON_Number) // 如果是数字
        {
            basic_template.window_width = json_window_width->valueint;
        }

        cJSON* json_window_height = cJSON_GetObjectItem(json_root, "window_height");
        if(json_window_height && json_window_height->type == cJSON_Number) // 如果是数字
        {
            basic_template.window_height = json_window_height->valueint;
        }
    }

    void
    parse_player_template(PlayerTemplate& player_template, cJSON* json_root)
    {
        if(!json_root || json_root->type != cJSON_Object) // 如果为空或者不是对象
        {
            return;
        }

        cJSON* json_speed = cJSON_GetObjectItem(json_root, "speed");
        if(json_speed && json_speed->type == cJSON_Number) // 如果是数字
        {
            player_template.speed = json_speed->valuedouble;
        }

        cJSON* json_normal_attack_interval = cJSON_GetObjectItem(json_root, "normal_attack_interval");
        if(json_normal_attack_interval && json_normal_attack_interval->type == cJSON_Number) // 如果是数字
        {
            player_template.normal_attack_interval = json_normal_attack_interval->valuedouble;
        }

        cJSON* json_normal_attack_damage = cJSON_GetObjectItem(json_root, "normal_attack_damage");
        if(json_normal_attack_damage && json_normal_attack_damage->type == cJSON_Number) // 如果是数字
        {
            player_template.normal_attack_damage = json_normal_attack_damage->valuedouble;
        }

        cJSON* json_skill_interval = cJSON_GetObjectItem(json_root, "skill_interval");
        if(json_skill_interval && json_skill_interval->type == cJSON_Number) // 如果是数字
        {
            player_template.skill_interval = json_skill_interval->valuedouble;
        }

        cJSON* json_skill_damage = cJSON_GetObjectItem(json_root, "skill_damage");
        if(json_skill_damage && json_skill_damage->type == cJSON_Number) // 如果是数字
        {
            player_template.skill_damage = json_skill_damage->valuedouble;
        }
    }

    void
    parse_number_array(double* array, int max_len, cJSON* json_root)
    {
        if(!json_root || json_root->type != cJSON_Array) // 如果为空或者不是数组
        {
            return;
        }

        int idx = -1;

        cJSON* json_element = nullptr;
        cJSON_ArrayForEach(json_element, json_root)
        {
            if(json_element->type != cJSON_Number || ++idx >= max_len) // 如果不是数字或者idx+1超过最大长度
            {
                continue;
            }

            array[idx] = json_element->valuedouble; // 将值赋给数组
        }
    }

    void
    parse_tower_template(TowerTemplate& tower_template, cJSON* json_root)
    {
        if(!json_root || json_root->type != cJSON_Object) // 如果为空或者不是对象
        {
            return;
        }

        cJSON* json_interval = cJSON_GetObjectItem(json_root, "interval");
        parse_number_array(tower_template.interval, 10, json_interval);

        cJSON* json_damage = cJSON_GetObjectItem(json_root, "damage");
        parse_number_array(tower_template.damage, 10, json_damage);

        cJSON* json_view_range = cJSON_GetObjectItem(json_root, "view_range");
        parse_number_array(tower_template.view_range, 10, json_view_range);

        cJSON* json_cost = cJSON_GetObjectItem(json_root, "cost");
        parse_number_array(tower_template.cost, 10, json_cost);

        cJSON* json_upgrade_cost = cJSON_GetObjectItem(json_root, "upgrade_cost");
        parse_number_array(tower_template.upgrade_cost, 9, json_upgrade_cost);
    }

    void
    parse_enemy_template(EnemyTemplate& enemy_template, cJSON* json_root)
    {
        if(!json_root || json_root->type != cJSON_Object) // 如果为空或者不是对象
        {
            return;
        }

        cJSON* json_hp = cJSON_GetObjectItem(json_root, "hp");
        if(json_hp && json_hp->type == cJSON_Number) // 如果是数字
        {
            enemy_template.hp = json_hp->valuedouble;
        }

        cJSON* json_speed = cJSON_GetObjectItem(json_root, "speed");
        if(json_speed && json_speed->type == cJSON_Number) // 如果是数字
        {
            enemy_template.speed = json_speed->valuedouble;
        }

        cJSON* json_damage = cJSON_GetObjectItem(json_root, "damage");
        if(json_damage && json_damage->type == cJSON_Number) // 如果是数字
        {
            enemy_template.damage = json_damage->valuedouble;
        }

        cJSON* json_reward_ratio = cJSON_GetObjectItem(json_root, "reward_ratio");
        if(json_reward_ratio && json_reward_ratio->type == cJSON_Number) // 如果是数字
        {
            enemy_template.reward_ratio = json_reward_ratio->valuedouble;
        }

        cJSON* json_recover_interval = cJSON_GetObjectItem(json_root, "recover_interval");
        if(json_recover_interval && json_recover_interval->type == cJSON_Number) // 如果是数字
        {
            enemy_template.recover_interval = json_recover_interval->valuedouble;
        }

        cJSON* json_recover_range = cJSON_GetObjectItem(json_root, "recover_range");
        if(json_recover_range && json_recover_range->type == cJSON_Number) // 如果是数字
        {
            enemy_template.recover_range = json_recover_range->valuedouble;
        }

        cJSON* json_recover_intensity = cJSON_GetObjectItem(json_root, "recover_intensity");
        if(json_recover_intensity && json_recover_intensity->type == cJSON_Number) // 如果是数字
        {
            enemy_template.recover_intensity = json_recover_intensity->valuedouble;
        }
    }
};
