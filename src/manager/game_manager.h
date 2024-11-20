
// game_manager.h

#pragma once

#include "manager.h"

#include "SDL.h"

#define FPS 60

class GameManager : public Manager<GameManager>
{
    friend class Manager<GameManager>;

public:
    int Run(int argc, char** argv);

protected:
    GameManager();
    ~GameManager();

private:
    bool is_running = true;

    SDL_Window*   window = nullptr;
    SDL_Event     event;
    SDL_Renderer* renderer     = nullptr;
    SDL_Texture*  tex_tile_map = nullptr;

private:
    void init_assert(bool flag, const char* msg);

    void on_input();
    void on_update(double delta_time);
    void on_render();

    bool generate_tile_map_texture(); // 生成地图纹理
};
