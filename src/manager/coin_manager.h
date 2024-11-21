
// coin_manager.h

#pragma once

#include "base.h"
#include "manager.h"

#include "SDL.h"

#include <vector>

class CoinProp
{
public:
    CoinProp();
    ~CoinProp() = default;

    void On_update(double delta_time);
    void On_render(SDL_Renderer* renderer);

    void Set_position(const Vector2& position);

    void Make_invalid();

    const Vector2& Get_size() const;
    const Vector2& Get_position() const;

    bool Can_remove() const;

private:
    bool is_valid   = true;
    bool is_jumping = false;

    double gravity            = 490;
    double interval_jump      = 0.75;
    double interval_disappear = 10;

    Vector2 size = { 16, 16 };
    Vector2 position;
    Vector2 velocity;

    Timer timer_jump;
    Timer timer_disappear;
};

class CoinManager : public Manager<CoinManager>
{
    friend class Manager<CoinManager>;

public:
    typedef std::vector<CoinProp*> CoinPropList;

public:
    void On_update(double delta_time);
    void On_render(SDL_Renderer* renderer) const;

    void Spawn_coin(const Vector2& position);

    void Increase_coin(double val);
    void Decrease_coin(double val);

    double        Get_num_coin() const;
    CoinPropList& Get_coin_prop_list();

protected:
    CoinManager();
    ~CoinManager();

private:
    double num_coin = 0;

    CoinPropList coin_prop_list;
};
