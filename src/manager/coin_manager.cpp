
// coin_manager.cpp

#include "coin_manager.h"

#include "config_manager.h"
#include "resources_manager.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>


// CoinProp
CoinProp::CoinProp()
{
    timer_jump.set_one_shot(true);
    timer_jump.set_wait_time(interval_jump);
    timer_jump.set_on_timeout([this]() { this->is_jumping = false; });

    timer_disappear.set_one_shot(true);
    timer_disappear.set_wait_time(interval_disappear);
    timer_disappear.set_on_timeout([this]() { this->is_valid = false; });

    velocity.vx = (rand() % 1000 - 500) / 500.0 * SIZE_TILE;
    velocity.vy = -3 * SIZE_TILE;
}

void
CoinProp::On_update(double delta_time)
{
    timer_jump.on_update(delta_time);
    timer_disappear.on_update(delta_time);

    if(is_jumping)
    {
        velocity.vy += gravity * delta_time;
    }
    else
    {
        velocity.vx = 0;
        velocity.vy = sin(SDL_GetTicks64() / 25.0) * 30; // 获取当前时间
    }

    position += velocity * delta_time;
}

void
CoinProp::On_render(SDL_Renderer* renderer)
{
    static SDL_Rect     rect_dst = { 0, 0, (int)size.vx, (int)size.vy };
    static SDL_Texture* tex_coin = ResourcesManager::Instance().get_texture_pool().find(ResID::Tex_Coin)->second;

    SDL_RenderCopy(renderer, tex_coin, nullptr, &rect_dst);
}

void
CoinProp::Set_position(const Vector2& position)
{
    this->position = position;
}

void
CoinProp::Make_invalid()
{
    is_valid = false;
}

const Vector2&
CoinProp::Get_size() const
{
    return size;
}

const Vector2&
CoinProp::Get_position() const
{
    return position;
}

bool
CoinProp::Can_remove() const
{
    return !is_valid;
}


// CoinManager
CoinManager::CoinManager()
{
    num_coin = ConfigManager::Instance().num_initial_coin;
}

CoinManager::~CoinManager()
{
    for(auto coin_prop : coin_prop_list)
    {
        delete coin_prop;
    }
}

void
CoinManager::On_update(double delta_time)
{
    for(auto coin_prop : coin_prop_list)
    {
        coin_prop->On_update(delta_time);
    }

    auto func = [](const CoinProp* coin_prop) {
        bool deletable = coin_prop->Can_remove();
        if(deletable)
        {
            delete coin_prop;
        }
        return deletable;
    };

    coin_prop_list.erase(std::remove_if(coin_prop_list.begin(), coin_prop_list.end(), func), coin_prop_list.end());
}

void
CoinManager::On_render(SDL_Renderer* renderer) const
{
    for(auto coin_prop : coin_prop_list)
    {
        coin_prop->On_render(renderer);
    }
}

void
CoinManager::Spawn_coin(const Vector2& position)
{
    CoinProp* coin_prop = new CoinProp();

    coin_prop->Set_position(position);
    coin_prop_list.push_back(coin_prop);
}

void
CoinManager::Increase_coin(double val)
{
    num_coin += val;
}

void
CoinManager::Decrease_coin(double val)
{
    num_coin -= val;

    if(num_coin < 0)
    {
        num_coin = 0;
    }
}

double
CoinManager::Get_num_coin() const
{
    return num_coin;
}

CoinManager::CoinPropList&
CoinManager::Get_coin_prop_list()
{
    return coin_prop_list;
}
