
// panel.cpp

#include "panel.h"

#include "config_manager.h"
#include "resources_manager.h"

#include <string>

Panel::Panel()
{
    tex_select_cursor = ResourcesManager::Instance().get_texture_pool().find(ResID::Tex_UISelectCursor)->second;
}

Panel::~Panel()
{
    SDL_DestroyTexture(tex_text_background);
    SDL_DestroyTexture(tex_text_foreground);
}

void
Panel::On_update(SDL_Renderer* renderer)
{
    static TTF_Font* font = ResourcesManager::Instance().get_font_pool().find(ResID::Font_Main)->second;

    if(hovered_target == HoveredTarget::None) return;

    int val = 0;
    switch(hovered_target)
    {
        case Panel::HoveredTarget::Top: val = val_top; break;
        case Panel::HoveredTarget::Left: val = val_left; break;
        case Panel::HoveredTarget::Right: val = val_right; break;
        default: break;
    }

    SDL_DestroyTexture(tex_text_background);
    tex_text_background = nullptr;
    SDL_DestroyTexture(tex_text_foreground);
    tex_text_foreground = nullptr;

    std::string str_val = val < 0 ? "MAX" : std::to_string(val);

    SDL_Surface* suf_text_background = TTF_RenderText_Blended(font, str_val.c_str(), color_text_background);
    SDL_Surface* suf_text_foreground = TTF_RenderText_Blended(font, str_val.c_str(), color_text_foreground);

    text_width  = suf_text_background->w;
    text_height = suf_text_background->h;

    tex_text_background = SDL_CreateTextureFromSurface(renderer, suf_text_background);
    tex_text_foreground = SDL_CreateTextureFromSurface(renderer, suf_text_foreground);

    SDL_FreeSurface(suf_text_background);
    SDL_FreeSurface(suf_text_foreground);
}

void
Panel::On_render(SDL_Renderer* renderer) const
{
    if(!is_visible) return;

    SDL_Rect rect_dst_cursor = { center_pos.x - (SIZE_TILE >> 1), center_pos.y - (SIZE_TILE >> 1), SIZE_TILE, SIZE_TILE };

    SDL_RenderCopy(renderer, tex_select_cursor, nullptr, &rect_dst_cursor);

    SDL_Rect rect_dst_panel = { center_pos.x - (width >> 1), center_pos.y - (height >> 1), width, height };

    SDL_Texture* tex_panel = nullptr;
    switch(hovered_target)
    {
        case Panel::HoveredTarget::None: tex_panel = tex_idle; break;
        case Panel::HoveredTarget::Top: tex_panel = tex_hovered_top; break;
        case Panel::HoveredTarget::Left: tex_panel = tex_hovered_left; break;
        case Panel::HoveredTarget::Right: tex_panel = tex_hovered_right; break;
        default: break;
    }

    SDL_RenderCopy(renderer, tex_panel, nullptr, &rect_dst_panel);

    if(hovered_target == HoveredTarget::None) return;

    SDL_Rect rect_dst_text = { 0, 0, text_width, text_height };

    // 画文本（背景）
    rect_dst_text.x = center_pos.x - (text_width >> 1) + offset_top.x;
    rect_dst_text.y = center_pos.y - (text_height >> 1) + offset_top.y;
    SDL_RenderCopy(renderer, tex_text_background, nullptr, &rect_dst_text);

    // 画文本（前景）
    rect_dst_text.x += offset_shadow.x;
    rect_dst_text.y += offset_shadow.y;
    SDL_RenderCopy(renderer, tex_text_foreground, nullptr, &rect_dst_text);
}

void
Panel::On_input(SDL_Event& event)
{
    if(!is_visible) return;

    switch(event.type)
    {
        case SDL_MOUSEMOTION: // 鼠标移动
        {
            SDL_Point pos_current = { event.motion.x, event.motion.y };
            SDL_Rect  rect_target = { 0, 0, button_size, button_size };

            rect_target.x = center_pos.x - (width >> 1) + offset_top.x;
            rect_target.y = center_pos.y - (height >> 1) + offset_top.y;
            if(SDL_PointInRect(&pos_current, &rect_target))
            {
                hovered_target = HoveredTarget::Top;
                return;
            }

            rect_target.x = center_pos.x - (width >> 1) + offset_left.x;
            rect_target.y = center_pos.y - (height >> 1) + offset_left.y;
            if(SDL_PointInRect(&pos_current, &rect_target))
            {
                hovered_target = HoveredTarget::Left;
                return;
            }

            rect_target.x = center_pos.x - (width >> 1) + offset_right.x;
            rect_target.y = center_pos.y - (height >> 1) + offset_right.y;
            if(SDL_PointInRect(&pos_current, &rect_target))
            {
                hovered_target = HoveredTarget::Right;
                return;
            }

            hovered_target = HoveredTarget::None;

            break;
        }

        case SDL_MOUSEBUTTONUP: // 鼠标抬起
        {
            switch(hovered_target)
            {
                case HoveredTarget::Top: on_click_top_area(); break;
                case HoveredTarget::Left: on_click_left_area(); break;
                case HoveredTarget::Right: on_click_right_area(); break;
                default: break;
            }
            break;

            is_visible = false;
        }

        default: break;
    }
}

void
Panel::Set_idx_tile(const SDL_Point& idx)
{
    idx_tile_selected = idx;
}

void
Panel::Set_center_pos(const SDL_Point& pos)
{
    center_pos = pos;
}

void
Panel::Make_visible()
{
    is_visible = true;
}
