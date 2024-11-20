
// coin_manager.h

#pragma once

#include "manager.h"

#include <vector>

class CoinManager : public Manager<CoinManager>
{
    friend class Manager<CoinManager>;

public:
    // typedef std::vector<CoinProp*> CoinPropList;

public:
    void Increase_coin(double val);
    void Decrease_coin(double val);

protected:
    CoinManager()  = default;
    ~CoinManager() = default;

private:
    double num_coin = 0;

    // CoinPropList coin_prop_list;
};
