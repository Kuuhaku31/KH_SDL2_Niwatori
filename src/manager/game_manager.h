
// game_manager.h

#pragma once

#include "banner.h"
#include "manager.h"
#include "panel.h"
#include "status_bar.h"

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

    StatusBar status_bar;

    SDL_Window*   window = nullptr;
    SDL_Event     event;
    SDL_Renderer* renderer     = nullptr;
    SDL_Texture*  tex_tile_map = nullptr;

    PlacePanel*   place_panel   = nullptr;
    UpgradePanel* upgrade_panel = nullptr;

    Banner* banner = nullptr;

private:
    void init_assert(bool flag, const char* msg);

    void on_input();
    void on_update(double delta_time);
    void on_render();

    bool generate_tile_map_texture(); // 生成地图纹理

    bool get_cursor_idx_tile(SDL_Point& idx_tile_selected, int screen_x, int screen_y);
    bool can_place_tower(const SDL_Point& idx_tile_selected);
    bool check_home(const SDL_Point& idx_tile_selected);

    void get_selected_tile_center_position(SDL_Point& center_pos, const SDL_Point& idx_tile_selected) const;
};
