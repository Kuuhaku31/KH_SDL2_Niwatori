
// tile.h

#pragma once

#include <vector>

#define SIZE_TILE 48 // 地图块大小（像素）

struct Tile
{
    enum class Direction
    {
        None = 0,
        Up,
        Down,
        Left,
        Right
    };

    int       terrain    = 0;  // 地形索引
    int       decoration = -1; // 装饰，-1表示没有装饰
    Direction direction  = Direction::None;
    int       special    = -1; // 特殊属性，-1表示没有特殊属性

    bool has_tower = false; // 是否有塔
};

// 自定义地图类型
typedef std::vector<std::vector<Tile>> TileMap;
