
// route.h

#pragma once

#include "tile.h"

#include "SDL.h"
#include <vector>

// 路径类
class Route
{
public:
    typedef std::vector<SDL_Point> IdxList;

public:
    Route() = default;
    Route(const TileMap& map, const SDL_Point& idx_origin)
    {
        size_t    map_width  = map[0].size();
        size_t    map_height = map.size();
        SDL_Point idx_next   = idx_origin;

        while(true)
        {
            // 如果超出地图范围，退出循环
            if(idx_next.x < 0 || idx_next.x >= map_width || idx_next.y < 0 || idx_next.y >= map_height)
            {
                break;
            }
            else if(check_duplicate_idx(idx_next)) // 如果索引重复，退出循环
            {
                break;
            }
            else
            {
                idx_list.push_back(idx_next);
            }

            bool is_next_dir_exist = true;

            const Tile& tile = map[idx_next.y][idx_next.x];
            if(tile.special == 0) // 家
            {
                break;
            }

            switch(tile.direction)
            {
                case Tile::Direction::Up:
                {
                    idx_next.y--;
                }
                break;

                case Tile::Direction::Down:
                {
                    idx_next.y++;
                }
                break;

                case Tile::Direction::Left:
                {
                    idx_next.x--;
                }
                break;

                case Tile::Direction::Right:
                {
                    idx_next.x++;
                }
                break;

                default:
                {
                    is_next_dir_exist = false;
                }
                break;
            }

            if(!is_next_dir_exist)
            {
                break;
            }
        }
    }

    ~Route() = default;

    const IdxList& Get_idx_list() const
    {
        return idx_list;
    }

private:
    IdxList idx_list; // 索引列表

private:
    // 检测索引是否重复
    bool
    check_duplicate_idx(const SDL_Point& target_idx)
    {
        for(auto& idx_temp : idx_list)
        {
            // 如果索引重复，返回true
            if(idx_temp.x == target_idx.x && idx_temp.y == target_idx.y)
            {
                return true;
            }
        }

        return false;
    }
};
