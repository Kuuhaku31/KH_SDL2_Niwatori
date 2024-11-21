
// banner.cpp

#include "banner.h"

#include "config_manager.h"
#include "resources_manager.h"

#define SIZE_BACKGROUND { 1282, 209 }
#define SIZE_FOREGROUND { 646, 215 }

Banner::Banner()
{
    size_background = SIZE_BACKGROUND;
    size_foreground = SIZE_FOREGROUND;

    timer_display.set_one_shot(true);
    timer_display.set_wait_time(5);
    timer_display.set_on_timeout([&]() { is_end_display = true; });
}

void
Banner::On_update(double delta_time)
{
    static const ResourcesManager::TexturePool& tex_pool = ResourcesManager::Instance().get_texture_pool();

    static const ConfigManager& config_manager = ConfigManager::Instance();

    timer_display.on_update(delta_time);

    texture_background = tex_pool.find(ResID::Tex_UIGameOverBar)->second;
    texture_foreground = tex_pool.find(config_manager.is_game_win ? ResID::Tex_UIWinText : ResID::Tex_UILossText)->second;
}

void
Banner::On_render(SDL_Renderer* renderer)
{
    SDL_Rect rect_background = {
        (int)(position.vx - size_background.vx / 2),
        (int)(position.vy - size_background.vy / 2),
        (int)size_background.vx,
        (int)size_background.vy
    };

    SDL_Rect rect_foreground = {
        (int)(position.vx - size_foreground.vx / 2),
        (int)(position.vy - size_foreground.vy / 2),
        (int)size_foreground.vx,
        (int)size_foreground.vy
    };

    SDL_RenderCopy(renderer, texture_background, nullptr, &rect_background);
    SDL_RenderCopy(renderer, texture_foreground, nullptr, &rect_foreground);
}

void
Banner::Set_center_position(const Vector2& position)
{
    this->position = position;
}

bool
Banner::Is_end_display() const
{
    return is_end_display;
}
