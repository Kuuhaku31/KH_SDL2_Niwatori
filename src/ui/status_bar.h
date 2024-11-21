
// status_bar.h

#pragma once

#include "base.h"
#include "coin_manager.h"
#include "home_manager.h"
// #include "player_manager.h"
#include "resources_manager.h"

#include "SDL.h"
#include "SDL2_gfxPrimitives.h"
#include "SDL_ttf.h"

#include <string>

class StatusBar
{
public:
    StatusBar()  = default;
    ~StatusBar() = default;

    void On_update(SDL_Renderer* renderer);
    void On_render(SDL_Renderer* renderer) const;

    void Set_position(int x, int y);

    void Increase_hp(double val);
    void Decrease_hp(double val);

    void Increase_coin(double val);
    void Decrease_coin(double val);

    void Increase_score(double val);
    void Decrease_score(double val);

    double Get_hp() const;
    double Get_coin() const;
    double Get_score() const;

private:
    const int       size_heart               = 32;
    const int       width_mp_bar             = 200;
    const int       height_mp_bar            = 20;
    const int       width_border_mp_bar      = 4;
    const SDL_Point offset_shadow            = { 2, 2 }; // 阴影偏移
    const SDL_Color color_text_background    = { 175, 175, 175, 255 };
    const SDL_Color color_text_foreground    = { 255, 255, 255, 255 };
    const SDL_Color color_mp_bar_background  = { 48, 40, 51, 255 };
    const SDL_Color color_mp_bar_foredground = { 144, 121, 173, 255 };

private:
    SDL_Point position = { 0, 0 };

    int width_text  = 0;
    int height_text = 0;

    SDL_Texture* tex_text_background = nullptr;
    SDL_Texture* tex_text_foreground = nullptr;
};
