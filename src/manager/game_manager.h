
// game_manager.h

#pragma once

#include "config_manager.h"
#include "manager.h"
#include "resources_manager.h"

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"

#include <cmath>

#define FPS 60

class GameManager : public Manager<GameManager>
{
    friend class Manager<GameManager>;

public:
    int
    Run(int argc, char** argv)
    {
        Uint64 last_counter = SDL_GetPerformanceCounter();   // 获取CPU计数器
        Uint64 counter_freq = SDL_GetPerformanceFrequency(); // 获取CPU频率
        // 事件循环
        while(is_running)
        {
            while(SDL_PollEvent(&event))
            {
                on_input();
            }

            Uint64 current_counter = SDL_GetPerformanceCounter();

            double delta = (double)(current_counter - last_counter) / (double)counter_freq;
            last_counter = current_counter;
            if(delta < 1.0 / FPS) SDL_Delay((1.0 / FPS - delta) * 1000);


            // 更新
            on_update();

            // 渲染
            on_render();
        }

        return 0;
    }

protected:
    GameManager()
    {
        // 检测初始化是否成功
        init_assert(!SDL_Init(SDL_INIT_EVERYTHING), (const char*)u8"SDL_Init failed!");
        init_assert(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG), (const char*)u8"IMG_Init failed!");
        init_assert(Mix_Init(MIX_INIT_MP3), (const char*)u8"Mix_Init failed!");
        init_assert(!TTF_Init(), (const char*)u8"TTF_Init failed!");

        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048); // 打开音频设备

        SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1"); // 设置输入法显示

        // 加载配置
        ConfigManager& config = ConfigManager::Instance();
        init_assert(config.map.Load("../config/map.csv"), (const char*)u8"加载游戏地图失败！");
        init_assert(config.Load_level_config("../config/level.json"), (const char*)u8"加载关卡配置失败！");
        init_assert(config.Load_game_config("../config/config.json"), (const char*)u8"加载游戏配置失败！");

        window = SDL_CreateWindow(
            config.basic_template.window_title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            config.basic_template.window_width,
            config.basic_template.window_height,
            SDL_WINDOW_SHOWN);
        init_assert(window, (const char*)u8"SDL_CreateWindow failed!");

        // 开启硬件加速、垂直同步、渲染目标纹理
        renderer = SDL_CreateRenderer(
            window,
            -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE); // 创建渲染器
        init_assert(renderer, (const char*)u8"SDL_CreateRenderer failed!");

        // 加载资源
        init_assert(ResourcesManager::Instance().load_from_file(renderer), (const char*)u8"加载游资源置失败！");
        init_assert(generate_tile_map_texture(), (const char*)u8"生成地图纹理失败！");
    }

    ~GameManager()
    {
        SDL_DestroyRenderer(renderer); // 销毁渲染器
        SDL_DestroyWindow(window);     // 销毁窗口

        Mix_CloseAudio(); // 关闭音频设备

        TTF_Quit(); // 退出TTF
        Mix_Quit(); // 退出Mixer
        IMG_Quit(); // 退出IMG
        SDL_Quit(); // 退出SDL
    }

private:
    SDL_Window* window = nullptr;

    SDL_Event event;
    bool      is_running = true;

    SDL_Renderer* renderer = nullptr;

    SDL_Texture* tex_tile_map = nullptr;

private:
    void
    init_assert(bool flag, const char* msg)
    {
        if(flag) return;

        // 弹出错误提示框
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!", msg, window);
        exit(-1);
    }

    void on_input()
    {
        // 输入处理
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                {
                    is_running = false;
                }
                break;
                default:
                {
                }
                break;
            }
        }
    }

    void on_update()
    {
        // 更新
    }

    void on_render()
    {
        // 渲染
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // 设置渲染颜色
        SDL_RenderClear(renderer);                      // 清空渲染目标

        static ConfigManager& config   = ConfigManager::Instance();
        static SDL_Rect&      rect_dis = config.rect_tile_map;

        SDL_RenderCopy(renderer, tex_tile_map, nullptr, &rect_dis); // 渲染地图纹理

        SDL_RenderPresent(renderer); // 显示渲染目标
    }

    // 生成地图纹理
    bool
    generate_tile_map_texture()
    {
        const Map&     map      = ConfigManager::Instance().map;
        const TileMap& tile_map = map.Get_tile_map();

        SDL_Rect&    rect_tile_map = ConfigManager::Instance().rect_tile_map;
        SDL_Texture& tex_tile_set  = *ResourcesManager::Instance().get_texture_pool().find(ResID::Tex_Tileset)->second;

        int tex_tile_set_width, tex_tile_set_height;
        SDL_QueryTexture(&tex_tile_set, nullptr, nullptr, &tex_tile_set_width, &tex_tile_set_height); // 获取纹理宽高

        int num_tile_single_line = (int)std::ceil((double)tex_tile_set_width / SIZE_TILE); // 计算单行瓦片数

        int tex_tile_map_width  = (int)map.Get_map_width() * SIZE_TILE;  // 计算地图纹理宽度
        int tex_tile_map_height = (int)map.Get_map_height() * SIZE_TILE; // 计算地图纹理高度

        tex_tile_map = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA8888, // 像素格式
            SDL_TEXTUREACCESS_TARGET, // 纹理访问权限
            tex_tile_map_width,
            tex_tile_map_height);
        if(!tex_tile_map) return false;

        ConfigManager& config = ConfigManager::Instance();

        // 设置渲染目标位置
        rect_tile_map.x = (config.basic_template.window_width - tex_tile_map_width) >> 1;
        rect_tile_map.y = (config.basic_template.window_height - tex_tile_map_height) >> 1;
        rect_tile_map.w = tex_tile_map_width;
        rect_tile_map.h = tex_tile_map_height;

        SDL_SetTextureBlendMode(tex_tile_map, SDL_BLENDMODE_BLEND); // 设置混合模式
        SDL_SetRenderTarget(renderer, tex_tile_map);                // 设置渲染目标

        for(int y = 0; y < map.Get_map_height(); y++)
        {
            for(int x = 0; x < map.Get_map_width(); x++)
            {
                const Tile& tile = tile_map[y][x];

                const SDL_Rect& dst_rect = { x * SIZE_TILE, y * SIZE_TILE, SIZE_TILE, SIZE_TILE }; // 目标矩形

                SDL_Rect src_rect = {
                    (tile.terrain % num_tile_single_line) * SIZE_TILE,
                    (tile.terrain / num_tile_single_line) * SIZE_TILE,
                    SIZE_TILE,
                    SIZE_TILE
                }; // 源矩形

                SDL_RenderCopy(renderer, &tex_tile_set, &src_rect, &dst_rect); // 渲染瓦片

                // 如果有装饰
                if(tile.decoration >= 0)
                {
                    src_rect = {
                        (tile.decoration % num_tile_single_line) * SIZE_TILE,
                        (tile.decoration / num_tile_single_line) * SIZE_TILE,
                        SIZE_TILE,
                        SIZE_TILE
                    }; // 源矩形

                    SDL_RenderCopy(renderer, &tex_tile_set, &src_rect, &dst_rect); // 渲染装饰
                }
            }
        }

        const SDL_Point& idx_home = map.Get_idx_home();
        const SDL_Rect&  dst_rect = { idx_home.x * SIZE_TILE, idx_home.y * SIZE_TILE, SIZE_TILE, SIZE_TILE };                        // 目标矩形
        SDL_RenderCopy(renderer, ResourcesManager::Instance().get_texture_pool().find(ResID::Tex_Home)->second, nullptr, &dst_rect); // 渲染家

        SDL_SetRenderTarget(renderer, nullptr); // 取消渲染目标

        return true;
    }
};
