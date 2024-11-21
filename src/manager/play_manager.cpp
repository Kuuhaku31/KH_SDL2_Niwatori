
// palyer_manager.cpp

#include "player_manager.h"

#include "coin_manager.h"
#include "config_manager.h"
#include "enemy_manager.h"
#include "resources_manager.h"

#define ANIMATION_FRAME_INTERVAL 0.1

#define PLAYER_SIZE { 96, 96 }

#define PLAYER_TEXTURE_CUT_X_Y 4, 8

#define PLAYER_EFFECT_IMPACT_UP_TEXTURE_CUT_X_Y 5, 1
#define PLAYER_EFFECT_IMPACT_DOWN_TEXTURE_CUT_X_Y 5, 1
#define PLAYER_EFFECT_IMPACT_LEFT_TEXTURE_CUT_X_Y 1, 5
#define PLAYER_EFFECT_IMPACT_RIGHT_TEXTURE_CUT_X_Y 1, 5

#define PLAYER_EFFECT_FLASH_UP_TEXTURE_CUT_X_Y 5, 1
#define PLAYER_EFFECT_FLASH_DOWN_TEXTURE_CUT_X_Y 5, 1
#define PLAYER_EFFECT_FLASH_LEFT_TEXTURE_CUT_X_Y 1, 5
#define PLAYER_EFFECT_FLASH_RIGHT_TEXTURE_CUT_X_Y 1, 5

#define PLAYER_ANIMATION_IDLE_UP_FRAME_INDEX { 4, 5, 6, 7 }
#define PLAYER_ANIMATION_IDLE_DOWN_FRAME_INDEX { 0, 1, 2, 3 }
#define PLAYER_ANIMATION_IDLE_LEFT_FRAME_INDEX { 8, 9, 10, 11 }
#define PLAYER_ANIMATION_IDLE_RIGHT_FRAME_INDEX { 12, 13, 14, 15 }

#define PLAYER_ANIMATION_ATTACK_UP_FRAME_INDEX { 20, 21 }
#define PLAYER_ANIMATION_ATTACK_DOWN_FRAME_INDEX { 16, 17 }
#define PLAYER_ANIMATION_ATTACK_LEFT_FRAME_INDEX { 24, 25 }
#define PLAYER_ANIMATION_ATTACK_RIGHT_FRAME_INDEX { 28, 29 }

#define PLAYER_EFFECT_FLASH_UP_FRAME_INDEX { 0, 1, 2, 3, 4 }
#define PLAYER_EFFECT_FLASH_DOWN_FRAME_INDEX { 4, 3, 2, 1, 0 }
#define PLAYER_EFFECT_FLASH_LEFT_FRAME_INDEX { 4, 3, 2, 1, 0 }
#define PLAYER_EFFECT_FLASH_RIGHT_FRAME_INDEX { 0, 1, 2, 3, 4 }

#define PLAYER_EFFECT_IMPACT_UP_FRAME_INDEX { 0, 1, 2, 3, 4 }
#define PLAYER_EFFECT_IMPACT_DOWN_FRAME_INDEX { 4, 3, 2, 1, 0 }
#define PLAYER_EFFECT_IMPACT_LEFT_FRAME_INDEX { 4, 3, 2, 1, 0 }
#define PLAYER_EFFECT_IMPACT_RIGHT_FRAME_INDEX { 0, 1, 2, 3, 4 }

PlayerManager::PlayerManager()
{
    static const ResourcesManager::TexturePool& tex_pool = ResourcesManager::Instance().get_texture_pool();

    static const ConfigManager& config_manager = ConfigManager::Instance();

    timer_auto_increase_mp.set_one_shot(false);
    timer_auto_increase_mp.set_wait_time(0.1);
    timer_auto_increase_mp.set_on_timeout(
        [&]() {
            double interval = config_manager.player_template.skill_interval;

            mp = std::min(mp + 100 / (interval / 0.1), 100.0);
        });

    timer_release_flash_cd.set_one_shot(true);
    timer_release_flash_cd.set_wait_time(ConfigManager::Instance().player_template.skill_interval);
    timer_release_flash_cd.set_on_timeout([&]() { can_release_flash = true; });

    SDL_Texture* tex_player = tex_pool.find(ResID::Tex_Player)->second;

    SDL_Texture* tex_flash_up    = tex_pool.find(ResID::Tex_EffectFlash_Up)->second;
    SDL_Texture* tex_flash_down  = tex_pool.find(ResID::Tex_EffectFlash_Down)->second;
    SDL_Texture* tex_flash_left  = tex_pool.find(ResID::Tex_EffectFlash_Left)->second;
    SDL_Texture* tex_flash_right = tex_pool.find(ResID::Tex_EffectFlash_Right)->second;

    SDL_Texture* tex_impact_up    = tex_pool.find(ResID::Tex_EffectImpact_Up)->second;
    SDL_Texture* tex_impact_down  = tex_pool.find(ResID::Tex_EffectImpact_Down)->second;
    SDL_Texture* tex_impact_left  = tex_pool.find(ResID::Tex_EffectImpact_Left)->second;
    SDL_Texture* tex_impact_right = tex_pool.find(ResID::Tex_EffectImpact_Right)->second;

    { // 闲置动画
        anim_idle_up.set_loop(true);
        anim_idle_up.set_interval(ANIMATION_FRAME_INTERVAL);
        anim_idle_up.add_frame(tex_player, PLAYER_TEXTURE_CUT_X_Y, PLAYER_ANIMATION_IDLE_UP_FRAME_INDEX);

        anim_idle_down.set_loop(true);
        anim_idle_down.set_interval(ANIMATION_FRAME_INTERVAL);
        anim_idle_down.add_frame(tex_player, PLAYER_TEXTURE_CUT_X_Y, PLAYER_ANIMATION_IDLE_DOWN_FRAME_INDEX);

        anim_idle_left.set_loop(true);
        anim_idle_left.set_interval(ANIMATION_FRAME_INTERVAL);
        anim_idle_left.add_frame(tex_player, PLAYER_TEXTURE_CUT_X_Y, PLAYER_ANIMATION_IDLE_LEFT_FRAME_INDEX);

        anim_idle_right.set_loop(true);
        anim_idle_right.set_interval(ANIMATION_FRAME_INTERVAL);
        anim_idle_right.add_frame(tex_player, PLAYER_TEXTURE_CUT_X_Y, PLAYER_ANIMATION_IDLE_RIGHT_FRAME_INDEX);
    }

    { // 攻击动画
        anim_attack_up.set_loop(true);
        anim_attack_up.set_interval(ANIMATION_FRAME_INTERVAL);
        anim_attack_up.add_frame(tex_player, PLAYER_TEXTURE_CUT_X_Y, PLAYER_ANIMATION_ATTACK_UP_FRAME_INDEX);

        anim_attack_down.set_loop(true);
        anim_attack_down.set_interval(ANIMATION_FRAME_INTERVAL);
        anim_attack_down.add_frame(tex_player, PLAYER_TEXTURE_CUT_X_Y, PLAYER_ANIMATION_ATTACK_DOWN_FRAME_INDEX);

        anim_attack_left.set_loop(true);
        anim_attack_left.set_interval(ANIMATION_FRAME_INTERVAL);
        anim_attack_left.add_frame(tex_player, PLAYER_TEXTURE_CUT_X_Y, PLAYER_ANIMATION_ATTACK_LEFT_FRAME_INDEX);

        anim_attack_right.set_loop(true);
        anim_attack_right.set_interval(ANIMATION_FRAME_INTERVAL);
        anim_attack_right.add_frame(tex_player, PLAYER_TEXTURE_CUT_X_Y, PLAYER_ANIMATION_ATTACK_RIGHT_FRAME_INDEX);
    }

    { // 特效动画（闪电）
        anim_effect_flash_up.set_loop(false);
        anim_effect_flash_up.set_interval(ANIMATION_FRAME_INTERVAL);
        anim_effect_flash_up.add_frame(tex_flash_up, PLAYER_EFFECT_FLASH_UP_TEXTURE_CUT_X_Y, PLAYER_EFFECT_FLASH_UP_FRAME_INDEX);
        anim_effect_flash_up.set_on_finished([&]() { is_releasing_flash = false; });

        anim_effect_flash_down.set_loop(false);
        anim_effect_flash_down.set_interval(ANIMATION_FRAME_INTERVAL);
        anim_effect_flash_down.add_frame(tex_flash_down, PLAYER_EFFECT_FLASH_DOWN_TEXTURE_CUT_X_Y, PLAYER_EFFECT_FLASH_DOWN_FRAME_INDEX);
        anim_effect_flash_down.set_on_finished([&]() { is_releasing_flash = false; });

        anim_effect_flash_left.set_loop(false);
        anim_effect_flash_left.set_interval(ANIMATION_FRAME_INTERVAL);
        anim_effect_flash_left.add_frame(tex_flash_left, PLAYER_EFFECT_FLASH_LEFT_TEXTURE_CUT_X_Y, PLAYER_EFFECT_FLASH_LEFT_FRAME_INDEX);
        anim_effect_flash_left.set_on_finished([&]() { is_releasing_flash = false; });

        anim_effect_flash_right.set_loop(false);
        anim_effect_flash_right.set_interval(ANIMATION_FRAME_INTERVAL);
        anim_effect_flash_right.add_frame(tex_flash_right, PLAYER_EFFECT_FLASH_RIGHT_TEXTURE_CUT_X_Y, PLAYER_EFFECT_FLASH_RIGHT_FRAME_INDEX);
        anim_effect_flash_right.set_on_finished([&]() { is_releasing_flash = false; });
    }

    { // 特效动画（冲击）
        anim_effect_impact_up.set_loop(false);
        anim_effect_impact_up.set_interval(ANIMATION_FRAME_INTERVAL);
        anim_effect_impact_up.add_frame(tex_impact_up, PLAYER_EFFECT_IMPACT_UP_TEXTURE_CUT_X_Y, PLAYER_EFFECT_IMPACT_UP_FRAME_INDEX);
        anim_effect_impact_up.set_on_finished([&]() { is_releasing_impact = false; });

        anim_effect_impact_down.set_loop(false);
        anim_effect_impact_down.set_interval(ANIMATION_FRAME_INTERVAL);
        anim_effect_impact_down.add_frame(tex_impact_down, PLAYER_EFFECT_IMPACT_DOWN_TEXTURE_CUT_X_Y, PLAYER_EFFECT_IMPACT_DOWN_FRAME_INDEX);
        anim_effect_impact_down.set_on_finished([&]() { is_releasing_impact = false; });

        anim_effect_impact_left.set_loop(false);
        anim_effect_impact_left.set_interval(ANIMATION_FRAME_INTERVAL);
        anim_effect_impact_left.add_frame(tex_impact_left, PLAYER_EFFECT_IMPACT_LEFT_TEXTURE_CUT_X_Y, PLAYER_EFFECT_IMPACT_LEFT_FRAME_INDEX);
        anim_effect_impact_left.set_on_finished([&]() { is_releasing_impact = false; });

        anim_effect_impact_right.set_loop(false);
        anim_effect_impact_right.set_interval(ANIMATION_FRAME_INTERVAL);
        anim_effect_impact_right.add_frame(tex_impact_right, PLAYER_EFFECT_IMPACT_RIGHT_TEXTURE_CUT_X_Y, PLAYER_EFFECT_IMPACT_RIGHT_FRAME_INDEX);
        anim_effect_impact_right.set_on_finished([&]() { is_releasing_impact = false; });
    }

    const SDL_Rect& rect_map = config_manager.rect_tile_map;

    position.vx = rect_map.x + (rect_map.w >> 1);
    position.vy = rect_map.y + (rect_map.h >> 1);

    speed = ConfigManager::Instance().player_template.speed;
    size  = PLAYER_SIZE;
}

void
PlayerManager::On_update(double delta_time)
{
    static const ConfigManager&               config_manager = ConfigManager::Instance();
    static const SDL_Rect&                    rect_map       = config_manager.rect_tile_map;
    static const ResourcesManager::SoundPool& sound_pool     = ResourcesManager::Instance().get_sound_pool();
    static EnemyManager::EnemyList&           enemy_list     = EnemyManager::Instance().Get_enemy_list();
    static CoinManager&                       coin_manager   = CoinManager::Instance();
    static CoinManager::CoinPropList&         coin_prop_list = coin_manager.Get_coin_prop_list();

    // 更新计时器
    timer_auto_increase_mp.on_update(delta_time);
    timer_release_flash_cd.on_update(delta_time);

    // 动画及移动逻辑
    if(!is_releasing_flash && !is_releasing_impact) // 未在释放技能
    {
        Vector2 direction = Vector2(is_move_right - is_move_left, is_move_down - is_move_up).unit();

        velocity = direction * speed * SIZE_TILE;
        position += velocity * delta_time;

        // 限制玩家移动范围，不超出地图
        if(position.vx < rect_map.x) position.vx = rect_map.x;
        if(position.vx > rect_map.x + rect_map.w) position.vx = rect_map.x + rect_map.w;
        if(position.vy < rect_map.y) position.vy = rect_map.y;
        if(position.vy > rect_map.y + rect_map.h) position.vy = rect_map.y + rect_map.h;

        // 根据速度方向设置玩家朝向
        if(velocity.vy > 0) facing = Facing::Down;
        if(velocity.vy < 0) facing = Facing::Up;
        if(velocity.vx > 0) facing = Facing::Right;
        if(velocity.vx < 0) facing = Facing::Left;

        // 根据朝向设置当前动画
        switch(facing)
        {
            case Facing::Left: anim_current = &anim_idle_left; break;
            case Facing::Right: anim_current = &anim_idle_right; break;
            case Facing::Up: anim_current = &anim_idle_up; break;
            case Facing::Down: anim_current = &anim_idle_down; break;
            default: anim_current = &anim_idle_right; break;
        }
    }
    else // 正在释放技能
    {
        switch(facing)
        {
            case Facing::Left: anim_current = &anim_attack_left; break;
            case Facing::Right: anim_current = &anim_attack_right; break;
            case Facing::Up: anim_current = &anim_attack_up; break;
            case Facing::Down: anim_current = &anim_attack_down; break;
            default: anim_current = &anim_attack_right; break;
        }
    }
    anim_current->on_update(delta_time);

    // 闪电技能逻辑
    if(is_releasing_flash)
    {
        anim_effect_flash_current->on_update(delta_time);

        for(Enemy* enemy : enemy_list)
        {
            if(enemy->Can_remove()) continue;

            const Vector2& position = enemy->Get_position();

            // 判断敌人是否在闪电范围内
            if(position.vx >= rect_hitbox_flash.x &&
               position.vx <= rect_hitbox_flash.x + rect_hitbox_flash.w &&
               position.vy >= rect_hitbox_flash.y &&
               position.vy <= rect_hitbox_flash.y + rect_hitbox_flash.h)
            {
                enemy->Decrease_hp(config_manager.player_template.normal_attack_damage * delta_time);
            }
        }
    }

    // 冲击技能逻辑
    if(is_releasing_impact)
    {
        anim_effect_impact_current->on_update(delta_time);

        for(Enemy* enemy : enemy_list)
        {
            if(enemy->Can_remove()) continue;

            const Vector2& size     = enemy->Get_size();
            const Vector2& position = enemy->Get_position();

            // 判断敌人是否在冲击范围内
            if(position.vx >= rect_hitbox_impact.x &&
               position.vx <= rect_hitbox_impact.x + rect_hitbox_impact.w &&
               position.vy >= rect_hitbox_impact.y &&
               position.vy <= rect_hitbox_impact.y + rect_hitbox_impact.h)
            {
                enemy->Decrease_hp(config_manager.player_template.skill_damage * delta_time);
                enemy->Slow_down();
            }
        }
    }

    // 捡金币逻辑
    for(CoinProp* coin_prop : coin_prop_list)
    {
        if(coin_prop->Can_remove()) continue;

        const Vector2& pos_coin_prop = coin_prop->Get_position();

        // 判断金币是否在玩家附近
        if(pos_coin_prop.vx >= position.vx - size.vx / 2 &&
           pos_coin_prop.vx <= position.vx + size.vx / 2 &&
           pos_coin_prop.vy >= position.vy - size.vy / 2 &&
           pos_coin_prop.vy <= position.vy + size.vy / 2)
        {
            coin_prop->Make_invalid();
            coin_manager.Increase_coin(10);

            Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_Coin)->second, 0);
        }
    }
}

void
PlayerManager::On_render(SDL_Renderer* renderer)
{
    static SDL_Point point;

    point.x = (int)(position.vx - size.vx / 2);
    point.y = (int)(position.vy - size.vy / 2);
    anim_current->on_render(renderer, point);

    if(is_releasing_flash)
    {
        point.x = rect_hitbox_flash.x;
        point.y = rect_hitbox_flash.y;
        anim_effect_flash_current->on_render(renderer, point);
    }

    if(is_releasing_impact)
    {
        point.x = rect_hitbox_impact.x;
        point.y = rect_hitbox_impact.y;
        anim_effect_impact_current->on_render(renderer, point);
    }
}

void
PlayerManager::On_input(const SDL_Event& event)
{
    switch(event.type)
    {
        case SDL_KEYDOWN:
        {
            switch(event.key.keysym.sym)
            {
                case SDLK_a: is_move_left = true; break;
                case SDLK_d: is_move_right = true; break;
                case SDLK_w: is_move_up = true; break;
                case SDLK_s: is_move_down = true; break;
                case SDLK_j: on_release_flash(); break;
                case SDLK_k: on_release_impact(); break;
                default: break;
            }
            break;
        }

        case SDL_KEYUP:
        {
            switch(event.key.keysym.sym)
            {
                case SDLK_a: is_move_left = false; break;
                case SDLK_d: is_move_right = false; break;
                case SDLK_w: is_move_up = false; break;
                case SDLK_s: is_move_down = false; break;
                default: break;
            }
            break;
        }
        default: break;
    }
}

double
PlayerManager::Get_current_mp() const
{
    return mp;
}

void
PlayerManager::on_release_flash()
{
    static const ResourcesManager::SoundPool& sound_pool = ResourcesManager::Instance().get_sound_pool();

    if(!can_release_flash || is_releasing_flash) return;

    // 根据玩家朝向设置闪电特效动画及碰撞矩形
    switch(facing)
    {
        case Facing::Left:
        {
            anim_effect_flash_current = &anim_effect_flash_left;

            rect_hitbox_flash.x = (int)(position.vx - size.vx / 2 - 300);
            rect_hitbox_flash.y = (int)(position.vy - 68 / 2);
            rect_hitbox_flash.w = 300;
            rect_hitbox_flash.h = 68;
            break;
        }

        case Facing::Right:
        {
            anim_effect_flash_current = &anim_effect_flash_right;

            rect_hitbox_flash.x = (int)(position.vx + size.vx / 2);
            rect_hitbox_flash.y = (int)(position.vy - 68 / 2);
            rect_hitbox_flash.w = 300;
            rect_hitbox_flash.h = 68;
            break;
        }

        case Facing::Up:
        {
            anim_effect_flash_current = &anim_effect_flash_up;

            rect_hitbox_flash.x = (int)(position.vx - 68 / 2);
            rect_hitbox_flash.y = (int)(position.vy - size.vx / 2 - 300);
            rect_hitbox_flash.w = 68;
            rect_hitbox_flash.h = 300;
            break;
        }

        case Facing::Down:
        {
            anim_effect_flash_current = &anim_effect_flash_down;

            rect_hitbox_flash.x = (int)(position.vx - 68 / 2);
            rect_hitbox_flash.y = (int)(position.vy + size.vx / 2);
            rect_hitbox_flash.w = 68;
            rect_hitbox_flash.h = 300;
            break;
        }

        default: break;
    }

    is_releasing_flash = true;
    anim_effect_flash_current->reset();
    timer_release_flash_cd.restart();

    Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_Flash)->second, 0);
}

void
PlayerManager::on_release_impact()
{
    static const ResourcesManager::SoundPool& sound_pool = ResourcesManager::Instance().get_sound_pool();

    if(mp < 100 || is_releasing_impact) return;

    // 根据玩家朝向设置冲击特效动画及碰撞矩形
    switch(facing)
    {
        case Facing::Left:
        {
            anim_effect_impact_current = &anim_effect_impact_left;

            rect_hitbox_impact.x = (int)(position.vx - size.vx / 2 - 60);
            rect_hitbox_impact.y = (int)(position.vy - 140 / 2);
            rect_hitbox_impact.w = 60;
            rect_hitbox_impact.h = 140;
            break;
        }

        case Facing::Right:
        {
            anim_effect_impact_current = &anim_effect_impact_right;

            rect_hitbox_impact.x = (int)(position.vx + size.vx / 2);
            rect_hitbox_impact.y = (int)(position.vy - 140 / 2);
            rect_hitbox_impact.w = 60;
            rect_hitbox_impact.h = 140;
            break;
        }

        case Facing::Up:
        {
            anim_effect_impact_current = &anim_effect_impact_up;

            rect_hitbox_impact.x = (int)(position.vx - 140 / 2);
            rect_hitbox_impact.y = (int)(position.vy - size.vx / 2 - 60);
            rect_hitbox_impact.w = 140;
            rect_hitbox_impact.h = 60;
            break;
        }

        case Facing::Down:
        {
            anim_effect_impact_current = &anim_effect_impact_down;

            rect_hitbox_impact.x = (int)(position.vx - 140 / 2);
            rect_hitbox_impact.y = (int)(position.vy + size.vx / 2);
            rect_hitbox_impact.w = 140;
            rect_hitbox_impact.h = 60;
            break;
        }

        default: break;
    }

    is_releasing_impact = true;
    anim_effect_impact_current->reset();
    mp = 0;

    Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_Impact)->second, 0);
}
