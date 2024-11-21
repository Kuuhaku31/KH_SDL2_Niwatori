
// panel.h

#pragma once

#include "base.h"

#include "SDL.h"

class Panel
{
public:
    Panel();
    ~Panel();

    virtual void On_update(SDL_Renderer* renderer);
    virtual void On_render(SDL_Renderer* renderer) const;
    void         On_input(SDL_Event& event);

    void Set_idx_tile(const SDL_Point& idx);
    void Set_center_pos(const SDL_Point& pos);

    void Make_visible();

protected:
    enum class HoveredTarget // 鼠标悬停目标
    {
        None,
        Top,
        Left,
        Right
    };

protected:
    bool is_visible = false;

    SDL_Point idx_tile_selected; // 选中的瓦片索引
    SDL_Point center_pos;        // 中心位置

    SDL_Texture* tex_idle          = nullptr;
    SDL_Texture* tex_hovered_top   = nullptr;
    SDL_Texture* tex_hovered_left  = nullptr;
    SDL_Texture* tex_hovered_right = nullptr;
    SDL_Texture* tex_select_cursor = nullptr; // 选择光标

    int val_top   = 0;
    int val_left  = 0;
    int val_right = 0;

    HoveredTarget hovered_target = HoveredTarget::None;

protected:
    virtual void on_click_top_area()   = 0; // 纯虚函数
    virtual void on_click_left_area()  = 0;
    virtual void on_click_right_area() = 0;

private:
    const int button_size = 48;

    const int width  = 144;
    const int height = 144;

    const SDL_Point offset_top            = { 48, 6 };
    const SDL_Point offset_left           = { 8, 80 };
    const SDL_Point offset_right          = { 90, 80 };
    const SDL_Point offset_shadow         = { 3, 3 };
    const SDL_Color color_text_background = { 175, 175, 175, 255 };
    const SDL_Color color_text_foreground = { 255, 255, 255, 255 };

    int text_width  = 0;
    int text_height = 0;

    SDL_Texture* tex_text_background = nullptr;
    SDL_Texture* tex_text_foreground = nullptr;
};


// PlacePanel
class PlacePanel : public Panel
{
public:
    PlacePanel();
    ~PlacePanel() = default;

    void On_update(SDL_Renderer* renderer) override;
    void On_render(SDL_Renderer* renderer) const override;

protected:
    void on_click_top_area() override;
    void on_click_left_area() override;
    void on_click_right_area() override;

private:
    const SDL_Color color_region  = { 30, 80, 162, 75 }; // 边框颜色
    const SDL_Color color_content = { 0, 149, 217, 75 }; // 内容颜色

private:
    int reg_top   = 0; // 边界大小
    int reg_left  = 0;
    int reg_right = 0;
};


// UpgradePanel
class UpgradePanel : public Panel
{
public:
    UpgradePanel();
    ~UpgradePanel() = default;

    void On_update(SDL_Renderer* renderer) override;

protected:
    void on_click_top_area() override;
    void on_click_left_area() override;
    void on_click_right_area() override;
};
