
// home_manager.cpp

#include "home_manager.h"

#include "config_manager.h"
#include "resources_manager.h"

HomeManager::HomeManager()
{
    home_hp = ConfigManager::Instance().num_initial_hp;
}

double
HomeManager::Get_current_hp()
{
    return home_hp;
}

void
HomeManager::Decrease_hp(double val)
{
    static const ResourcesManager::SoundPool& sound_pool = ResourcesManager::Instance().get_sound_pool();

    home_hp -= val;

    if(home_hp < 0)
    {
        home_hp = 0;
    }

    // 播放音效
    Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_HomeHurt)->second, 0);
}
