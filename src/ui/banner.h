
// banner.h

#pragma once

#include "base.h"

#include "SDL.h"

class Banner
{
public:
    Banner();
    ~Banner() = default;

    void On_update(double delta_time);
    void On_render(SDL_Renderer* renderer);

    void Set_center_position(const Vector2& position);

    bool Is_end_display() const;

private:
    Vector2 position;
    Vector2 size_background;
    Vector2 size_foreground;

    SDL_Texture* texture_background = nullptr;
    SDL_Texture* texture_foreground = nullptr;

    bool  is_end_display = false;
    Timer timer_display;
};
