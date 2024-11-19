
// home_manager.h

#pragma once

#include "manager.h"

class HomeManager : public Manager<HomeManager>
{
    friend class Manager<HomeManager>;

public:
    double Get_current_hp();
    void   Decrease_hp(double val);

protected:
    HomeManager();
    ~HomeManager() = default;

private:
    double home_hp = 100;
};
