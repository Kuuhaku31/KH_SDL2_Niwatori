
// bullet_manager.cpp

#include "bullet_manager.h"

#include <algorithm>

BulletManager::~BulletManager()
{
    for(auto bullet : bullet_list)
    {
        delete bullet;
    }
}

void
BulletManager::On_update(double delta_time)
{
    for(auto bullet : bullet_list)
    {
        bullet->On_update(delta_time);
    }

    auto func = [](const Bullet* bullet) {
        bool deletable = bullet->Can_remove();
        if(deletable)
        {
            delete bullet;
        }
        return deletable;
    };

    // 移除无效子弹
    bullet_list.erase(std::remove_if(bullet_list.begin(), bullet_list.end(), func), bullet_list.end());
}

void
BulletManager::On_render(SDL_Renderer* renderer)
{
    for(auto bullet : bullet_list)
    {
        bullet->On_render(renderer);
    }
}

BulletManager::BulletList&
BulletManager::Get_bullet_list()
{
    return bullet_list;
}

void
BulletManager::Fire_bullet(BulletType type, const Vector2& pos, const Vector2& vel, double damage)
{
    Bullet* bullet = nullptr;

    switch(type)
    {
        case BulletType::Arrow: bullet = new ArrowBullet(); break;
        case BulletType::Axe: bullet = new AxeBullet(); break;
        case BulletType::Shell: bullet = new ShellBullet(); break;
        default: bullet = new ArrowBullet(); break;
    }

    bullet->Set_position(pos + Vector2{ SIZE_TILE / 2, 0 });
    bullet->Set_velocity(vel);
    bullet->Set_damage(damage);

    bullet_list.push_back(bullet);
}
