
// bullet_manager.h

#pragma once

#include "bullet.h"
#include "bullet_type.h"
#include "manager.h"

#include <vector>

class BulletManager : public Manager<BulletManager>
{
    friend class Manager<BulletManager>;

public:
    typedef std::vector<Bullet*> BulletList;

public:
    void On_update(double delta_time);
    void On_render(SDL_Renderer* renderer);

    BulletList& Get_bullet_list(); // 获取子弹列表

    void Fire_bullet(BulletType type, const Vector2& pos, const Vector2& vel, double damage); // 生成子弹

protected:
    BulletManager() = default;
    ~BulletManager();

private:
    BulletList bullet_list;
};
