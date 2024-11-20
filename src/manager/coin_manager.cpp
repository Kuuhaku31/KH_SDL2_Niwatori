
// coin_manager.cpp

#include "coin_manager.h"

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
