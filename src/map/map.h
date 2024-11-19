
// map.h

#pragma once

#include "route.h"
#include "tile.h"

#include <SDL.h>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

class Map
{
public:
    typedef std::unordered_map<int, Route> SpawnRouteMap; // 生成路径映射

public:
    Map()  = default;
    ~Map() = default;

    bool
    Load(const char* file_path)
    {
        TileMap taile_map_temp;

        std::ifstream file(file_path);
        if(file.good())
        {
            int idx_x, idx_y = -1;

            std::string str_line;
            while(std::getline(file, str_line)) // 逐行读取
            {
                str_line = trim_str(str_line); // 处理前后空白字符
                if(str_line.empty()) continue;

                idx_x = -1, idx_y++;
                taile_map_temp.emplace_back();

                std::istringstream iss_line(str_line);

                std::string str_tile;
                while(std::getline(iss_line, str_tile, ',')) // 逐个读取
                {
                    idx_x++;
                    taile_map_temp[idx_y].emplace_back();
                    Tile& tile = taile_map_temp[idx_y].back();

                    str_tile = trim_str(str_tile); // 处理前后空白字符
                    load_tile_from_string(tile, str_tile);
                }
            }

            file.close();
        }
        else
        {
            return false;
        }

        if(taile_map_temp.empty() || taile_map_temp[0].empty()) // 检查地图是否为空
        {
            return false;
        }
        else
        {
            tile_map = taile_map_temp;

            generate_map_cache();

            return true;
        }
    }

    size_t
    Get_map_width() const
    {
        if(tile_map.empty())
        {
            return 0;
        }
        else
        {
            return tile_map[0].size();
        }
    }

    size_t
    Get_map_height() const
    {
        return tile_map.size();
    }

    const TileMap&
    Get_tile_map() const
    {
        return tile_map;
    }

    const SDL_Point&
    Get_idx_home() const
    {
        return idx_home;
    }

    const SpawnRouteMap&
    Get_spawn_route_map() const
    {
        return spawn_route_map;
    }

    void
    Place_tower(const SDL_Point& idx_tile)
    {
        if(idx_tile.x < 0 || idx_tile.x >= Get_map_width() || idx_tile.y < 0 || idx_tile.y >= Get_map_height())
        {
            return;
        }
        else
        {
            tile_map[idx_tile.y][idx_tile.x].has_tower = true;
        }
    }

private:
    TileMap tile_map;

    SDL_Point idx_home = { 0 }; // 家的索引位置

    SpawnRouteMap spawn_route_map; // 生成路径映射

private:
    // 处理字符串前后空白字符
    std::string
    trim_str(const std::string& str)
    {
        size_t begin_idx = str.find_first_not_of(" \t"); // 查找第一个非空白字符
        if(begin_idx == std::string::npos) return "";

        size_t end_idx = str.find_last_not_of(" \t"); // 查找最后一个非空白字符

        size_t idx_range = end_idx - begin_idx + 1; // 计算字符长度

        return str.substr(begin_idx, idx_range);
    }

    void
    load_tile_from_string(Tile& tile, const std::string& str_tile)
    {
        std::string       str_value;
        std::vector<int>  values;
        std::stringstream str_stream(str_tile);

        while(std::getline(str_stream, str_value, '\\'))
        {
            int value = 0;
            try // 将字符串转换为整数
            {
                value = std::stoi(str_value);
            }
            catch(const std::invalid_argument&)
            {
                value = -1;
            }

            values.push_back(value);
        }

        // 处理可能的异常情况
        tile.terrain    = (values.size() < 1 || values[0] < 0) ? 0 : values[0];
        tile.decoration = (values.size() < 2) ? -1 : values[1];
        tile.direction  = (Tile::Direction)((values.size() < 3 || values[2] < 0 || values[2] > 4) ? 0 : values[2]);
        tile.special    = (values.size() < 4) ? -1 : values[3];
    }

    // 生成地图缓存
    void
    generate_map_cache()
    {
        int map_width  = Get_map_width();
        int map_height = Get_map_height();

        for(int y = 0; y < map_height; y++)
        {
            for(int x = 0; x < map_width; x++)
            {
                Tile& tile = tile_map[y][x];

                if(tile.special < 0)
                {
                    continue;
                }
                else if(tile.special == 0) // 家
                {
                    idx_home.x = x;
                    idx_home.y = y;
                }
                else
                {
                    spawn_route_map[tile.special] = Route(tile_map, { x, y }); // 生成路径，存入映射
                }
            }
        }
    }
};
