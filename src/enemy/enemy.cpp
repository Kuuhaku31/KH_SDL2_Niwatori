
// enemy.cpp

#include "enemy.h"

#include "config_manager.h"
#include "resources_manager.h"


#define ENEMY_TIMER_SKETCH 0.075
#define ENEMY_SLOW_DOWN_SPEED 0.5

#define ANIMATION_FRAME_INTERVAL 0.1

// Slime 相关常量
#define SLIME_ANIMATION_IDX_LIST_UP { 6, 7, 8, 9, 10, 11 }
#define SLIME_ANIMATION_IDX_LIST_DOWN { 0, 1, 2, 3, 4, 5 }
#define SLIME_ANIMATION_IDX_LIST_LEFT { 18, 19, 20, 21, 22, 23 }
#define SLIME_ANIMATION_IDX_LIST_RIGHT { 12, 13, 14, 15, 16, 17 }
#define SLIME_TEXTURE_X_Y 6, 4
#define SLIME_SIZE { 48, 48 }

// KingSlime 相关常量
#define KING_SLIME_ANIMATION_IDX_LIST_UP { 18, 19, 20, 21, 22, 23 }
#define KING_SLIME_ANIMATION_IDX_LIST_DOWN { 0, 1, 2, 3, 4, 5 }
#define KING_SLIME_ANIMATION_IDX_LIST_LEFT { 6, 7, 8, 9, 10, 11 }
#define KING_SLIME_ANIMATION_IDX_LIST_RIGHT { 12, 13, 14, 15, 16, 17 }
#define KING_SLIME_TEXTURE_X_Y 6, 4
#define KING_SLIME_SIZE { 48, 48 }

// Skeleton 相关常量
#define SKELETON_ANIMATION_IDX_LIST_UP { 5, 6, 7, 8, 9 }
#define SKELETON_ANIMATION_IDX_LIST_DOWN { 0, 1, 2, 3, 4 }
#define SKELETON_ANIMATION_IDX_LIST_LEFT { 15, 16, 17, 18, 19 }
#define SKELETON_ANIMATION_IDX_LIST_RIGHT { 10, 11, 12, 13, 14 }
#define SKELETON_TEXTURE_X_Y 5, 4
#define SKELETON_SIZE { 48, 48 }

// Goblin 相关常量
#define GOBLIN_ANIMATION_IDX_LIST_UP { 5, 6, 7, 8, 9 }
#define GOBLIN_ANIMATION_IDX_LIST_DOWN { 0, 1, 2, 3, 4 }
#define GOBLIN_ANIMATION_IDX_LIST_LEFT { 15, 16, 17, 18, 19 }
#define GOBLIN_ANIMATION_IDX_LIST_RIGHT { 10, 11, 12, 13, 14 }
#define GOBLIN_TEXTURE_X_Y 5, 4
#define GOBLIN_SIZE { 48, 48 }

// GoblinPriest 相关常量
#define GOBLIN_PRIEST_ANIMATION_IDX_LIST_UP { 5, 6, 7, 8, 9 }
#define GOBLIN_PRIEST_ANIMATION_IDX_LIST_DOWN { 0, 1, 2, 3, 4 }
#define GOBLIN_PRIEST_ANIMATION_IDX_LIST_LEFT { 15, 16, 17, 18, 19 }
#define GOBLIN_PRIEST_ANIMATION_IDX_LIST_RIGHT { 10, 11, 12, 13, 14 }
#define GOBLIN_PRIEST_TEXTURE_X_Y 5, 4
#define GOBLIN_PRIEST_SIZE { 48, 48 }

Enemy::Enemy()
{
    timer_skill.set_one_shot(false);
    timer_skill.set_on_timeout([&]() { on_skill_released(this); });

    timer_sketch.set_one_shot(true);
    timer_sketch.set_wait_time(ENEMY_TIMER_SKETCH);
    timer_sketch.set_on_timeout([&]() { is_show_sketch = false; });

    timer_restore_speed.set_one_shot(true);
    timer_restore_speed.set_on_timeout([&]() { speed = max_speed; });
}

void
Enemy::refresh_position_target()
{
    if(idx_target < route->Get_idx_list().size())
    {
        static const SDL_Rect& rect_tile_map = ConfigManager::Instance().rect_tile_map;

        const SDL_Point& point = route->Get_idx_list()[idx_target];

        // 设置目标位置：地图左上角 + 点的坐标 * 瓦片大小 + 瓦片大小的一半
        position_target.vx = rect_tile_map.x + point.x * SIZE_TILE + (SIZE_TILE >> 1);
        position_target.vy = rect_tile_map.y + point.y * SIZE_TILE + (SIZE_TILE >> 1);
    }
}

void
Enemy::On_update(double delta_time)
{
    timer_skill.on_update(delta_time);
    timer_sketch.on_update(delta_time);
    timer_restore_speed.on_update(delta_time);

    Vector2 move_distance   = velocity * delta_time;      // 移动距离
    Vector2 target_distance = position_target - position; // 目标距离

    // 移动，如果移动距离小于目标距离，则移动距离，否则移动目标距离
    position += move_distance < target_distance ? move_distance : target_distance;

    // 如果目标距离近似为 0
    if(target_distance.approx_zero())
    {
        idx_target++;
        refresh_position_target();

        direction = (position_target - position).unit();
    }

    // 设置速度
    velocity = direction * (speed * SIZE_TILE);

    // 判断是否显示 x 轴动画
    if(std::abs(velocity.vx) >= std::abs(velocity.vy))
    {
        if(is_show_sketch)
        {
            animation_current = velocity.vx > 0 ? &animation_right_sketch : &animation_left_sketch;
        }
        else
        {
            animation_current = velocity.vx > 0 ? &animation_right : &animation_left;
        }
    }
    else
    {
        if(is_show_sketch)
        {
            animation_current = velocity.vy > 0 ? &animation_down_sketch : &animation_up_sketch;
        }
        else
        {
            animation_current = velocity.vy > 0 ? &animation_down : &animation_up;
        }
    }

    animation_current->on_update(delta_time);
}

void
Enemy::On_render(SDL_Renderer* renderer)
{
    static SDL_Point pos_display;

    // 设置显示位置
    pos_display.x = (int)(position.vx - size.vx / 2);
    pos_display.y = (int)(position.vy - size.vy / 2);

    animation_current->on_render(renderer, pos_display); // 不用设置角度

    // 如果生命值小于最大生命值，才显示生命值条
    if(hp < max_hp)
    {
        static const SDL_Color color_content = { 226, 255, 194, 255 }; // 填充颜色
        static const SDL_Color color_border  = { 116, 185, 124, 255 }; // 边框颜色
        static const Vector2   SIZE_HP_BAR   = { 40, 8 };
        static const int       OFFSET_Y      = 2;

        static SDL_Rect rect;

        // 设置生命值条的位置
        rect.x = (int)(position.vx - SIZE_HP_BAR.vx / 2);
        rect.y = (int)(position.vy - size.vy / 2 - SIZE_HP_BAR.vy - OFFSET_Y);
        rect.w = (int)(SIZE_HP_BAR.vx * (hp / max_hp));
        rect.h = (int)(SIZE_HP_BAR.vy);

        // 设置填充颜色
        SDL_SetRenderDrawColor(renderer, color_content.r, color_content.g, color_content.b, color_content.a);
        SDL_RenderFillRect(renderer, &rect); // 填充矩形

        // 设置边框颜色
        rect.w = (int)SIZE_HP_BAR.vx;
        SDL_SetRenderDrawColor(renderer, color_border.r, color_border.g, color_border.b, color_border.a);
        SDL_RenderDrawRect(renderer, &rect); // 绘制矩形（边框）
    }
}

bool
Enemy::Can_remove() const
{
    return !is_alive;
}

double
Enemy::Get_route_process() const
{
    if(route->Get_idx_list().size() == 1)
    {
        return 1;
    }
    else
    {
        return (double)idx_target / (route->Get_idx_list().size() - 1);
    }
}

void
Enemy::Increase_hp(double val)
{
    hp += val;

    if(hp > max_hp)
    {
        hp = max_hp;
    }
}

void
Enemy::Decrease_hp(double val)
{
    hp -= val;

    if(hp <= 0)
    {
        hp       = 0;
        is_alive = false;
    }

    is_show_sketch = true;
    timer_sketch.restart();
}

void
Enemy::Slow_down()
{
    speed = max_speed * ENEMY_SLOW_DOWN_SPEED;
    timer_restore_speed.restart();
}

void
Enemy::Set_on_skill_released(SkillCallback on_skill_released)
{
    this->on_skill_released = on_skill_released;
}


void
Enemy::Set_position(const Vector2& position)
{
    this->position = position;
}

void
Enemy::Set_route(const Route* route)
{
    this->route = route;

    idx_target = 0;
    refresh_position_target();
}

void
Enemy::Make_invalid()
{
    is_alive = false;
}

double
Enemy::Get_hp() const
{
    return hp;
}

const Vector2&
Enemy::Get_position() const
{
    return position;
}

const Vector2&
Enemy::Get_size() const
{
    return size;
}

const Vector2&
Enemy::Get_velocity() const
{
    return velocity;
}

double
Enemy::Get_damage() const
{
    return damage;
}

double
Enemy::Get_reward_ratio() const
{
    return reward_ratio;
}

double
Enemy::Get_recover_radius() const
{
    return SIZE_TILE * recover_range;
}

double
Enemy::Get_recover_intensity() const
{
    return recover_intensity;
}


// SlimEnemy
SlimEnemy::SlimEnemy()
{
    // 数据模板
    static ConfigManager::EnemyTemplate& slim_template = ConfigManager::Instance().slim_template;

    // 获取纹理池
    static const ResourcesManager::TexturePool& texture_pool = ResourcesManager::Instance().get_texture_pool();

    // 获取纹理
    static SDL_Texture* tex_slime        = texture_pool.find(ResID::Tex_Slime)->second;
    static SDL_Texture* tex_slime_sketch = texture_pool.find(ResID::Tex_SlimeSketch)->second;

    // 动画帧序列索引
    static const std::vector<int> idx_list_up    = SLIME_ANIMATION_IDX_LIST_UP;
    static const std::vector<int> idx_list_down  = SLIME_ANIMATION_IDX_LIST_DOWN;
    static const std::vector<int> idx_list_left  = SLIME_ANIMATION_IDX_LIST_LEFT;
    static const std::vector<int> idx_list_right = SLIME_ANIMATION_IDX_LIST_RIGHT;


    // 设置向上动画
    animation_up.set_loop(true);
    animation_up.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_up.add_frame(tex_slime, SLIME_TEXTURE_X_Y, idx_list_up);

    // 设置向下动画
    animation_down.set_loop(true);
    animation_down.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_down.add_frame(tex_slime, SLIME_TEXTURE_X_Y, idx_list_down);

    // 设置向左动画
    animation_left.set_loop(true);
    animation_left.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_left.add_frame(tex_slime, SLIME_TEXTURE_X_Y, idx_list_left);

    // 设置向右动画
    animation_right.set_loop(true);
    animation_right.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_right.add_frame(tex_slime, SLIME_TEXTURE_X_Y, idx_list_right);

    // 设置向上轮廓动画
    animation_up_sketch.set_loop(true);
    animation_up_sketch.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_up_sketch.add_frame(tex_slime_sketch, SLIME_TEXTURE_X_Y, idx_list_up);

    // 设置向下轮廓动画
    animation_down_sketch.set_loop(true);
    animation_down_sketch.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_down_sketch.add_frame(tex_slime_sketch, SLIME_TEXTURE_X_Y, idx_list_down);

    // 设置向左轮廓动画
    animation_left_sketch.set_loop(true);
    animation_left_sketch.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_left_sketch.add_frame(tex_slime_sketch, SLIME_TEXTURE_X_Y, idx_list_left);

    // 设置向右轮廓动画
    animation_right_sketch.set_loop(true);
    animation_right_sketch.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_right_sketch.add_frame(tex_slime_sketch, SLIME_TEXTURE_X_Y, idx_list_right);


    // 设置属性
    max_hp            = slim_template.hp;
    max_speed         = slim_template.speed;
    damage            = slim_template.damage;
    reward_ratio      = slim_template.reward_ratio;
    recover_interval  = slim_template.recover_interval;
    recover_range     = slim_template.recover_range;
    recover_intensity = slim_template.recover_intensity;

    size = SLIME_SIZE;

    hp    = max_hp;
    speed = max_speed;
}


// KingSlimeEnemy
KingSlimeEnemy::KingSlimeEnemy()
{
    // 数据模板
    static ConfigManager::EnemyTemplate& king_slim_template = ConfigManager::Instance().king_slim_template;

    // 获取纹理池
    static const ResourcesManager::TexturePool& texture_pool = ResourcesManager::Instance().get_texture_pool();

    // 获取纹理
    static SDL_Texture* tex_king_slime        = texture_pool.find(ResID::Tex_KingSlime)->second;
    static SDL_Texture* tex_king_slime_sketch = texture_pool.find(ResID::Tex_KingSlimeSketch)->second;

    // 动画帧序列索引
    static const std::vector<int> idx_list_up    = KING_SLIME_ANIMATION_IDX_LIST_UP;
    static const std::vector<int> idx_list_down  = KING_SLIME_ANIMATION_IDX_LIST_DOWN;
    static const std::vector<int> idx_list_left  = KING_SLIME_ANIMATION_IDX_LIST_LEFT;
    static const std::vector<int> idx_list_right = KING_SLIME_ANIMATION_IDX_LIST_RIGHT;


    // 设置向上动画
    animation_up.set_loop(true);
    animation_up.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_up.add_frame(tex_king_slime, KING_SLIME_TEXTURE_X_Y, idx_list_up);

    // 设置向下动画
    animation_down.set_loop(true);
    animation_down.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_down.add_frame(tex_king_slime, KING_SLIME_TEXTURE_X_Y, idx_list_down);

    // 设置向左动画
    animation_left.set_loop(true);
    animation_left.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_left.add_frame(tex_king_slime, KING_SLIME_TEXTURE_X_Y, idx_list_left);

    // 设置向右动画
    animation_right.set_loop(true);
    animation_right.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_right.add_frame(tex_king_slime, KING_SLIME_TEXTURE_X_Y, idx_list_right);

    // 设置向上轮廓动画
    animation_up_sketch.set_loop(true);
    animation_up_sketch.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_up_sketch.add_frame(tex_king_slime_sketch, KING_SLIME_TEXTURE_X_Y, idx_list_up);

    // 设置向下轮廓动画
    animation_down_sketch.set_loop(true);
    animation_down_sketch.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_down_sketch.add_frame(tex_king_slime_sketch, KING_SLIME_TEXTURE_X_Y, idx_list_down);

    // 设置向左轮廓动画
    animation_left_sketch.set_loop(true);
    animation_left_sketch.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_left_sketch.add_frame(tex_king_slime_sketch, KING_SLIME_TEXTURE_X_Y, idx_list_left);

    // 设置向右轮廓动画
    animation_right_sketch.set_loop(true);
    animation_right_sketch.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_right_sketch.add_frame(tex_king_slime_sketch, KING_SLIME_TEXTURE_X_Y, idx_list_right);


    // 设置属性
    max_hp            = king_slim_template.hp;
    max_speed         = king_slim_template.speed;
    damage            = king_slim_template.damage;
    reward_ratio      = king_slim_template.reward_ratio;
    recover_interval  = king_slim_template.recover_interval;
    recover_range     = king_slim_template.recover_range;
    recover_intensity = king_slim_template.recover_intensity;

    size = KING_SLIME_SIZE;

    hp    = max_hp;
    speed = max_speed;
}


// SkeletonEnemy
SkeletonEnemy::SkeletonEnemy()
{
    // 数据模板
    static ConfigManager::EnemyTemplate& skeleton_template = ConfigManager::Instance().skeleton_template;

    // 获取纹理池
    static const ResourcesManager::TexturePool& texture_pool = ResourcesManager::Instance().get_texture_pool();

    // 获取纹理
    static SDL_Texture* tex_skeleton        = texture_pool.find(ResID::Tex_Skeleton)->second;
    static SDL_Texture* tex_skeleton_sketch = texture_pool.find(ResID::Tex_SkeletonSketch)->second;

    // 动画帧序列索引
    static const std::vector<int> idx_list_up    = SKELETON_ANIMATION_IDX_LIST_UP;
    static const std::vector<int> idx_list_down  = SKELETON_ANIMATION_IDX_LIST_DOWN;
    static const std::vector<int> idx_list_left  = SKELETON_ANIMATION_IDX_LIST_LEFT;
    static const std::vector<int> idx_list_right = SKELETON_ANIMATION_IDX_LIST_RIGHT;


    // 设置向上动画
    animation_up.set_loop(true);
    animation_up.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_up.add_frame(tex_skeleton, SKELETON_TEXTURE_X_Y, idx_list_up);

    // 设置向下动画
    animation_down.set_loop(true);
    animation_down.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_down.add_frame(tex_skeleton, SKELETON_TEXTURE_X_Y, idx_list_down);

    // 设置向左动画
    animation_left.set_loop(true);
    animation_left.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_left.add_frame(tex_skeleton, SKELETON_TEXTURE_X_Y, idx_list_left);

    // 设置向右动画
    animation_right.set_loop(true);
    animation_right.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_right.add_frame(tex_skeleton, SKELETON_TEXTURE_X_Y, idx_list_right);

    // 设置向上轮廓动画
    animation_up_sketch.set_loop(true);
    animation_up_sketch.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_up_sketch.add_frame(tex_skeleton_sketch, SKELETON_TEXTURE_X_Y, idx_list_up);

    // 设置向下轮廓动画
    animation_down_sketch.set_loop(true);
    animation_down_sketch.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_down_sketch.add_frame(tex_skeleton_sketch, SKELETON_TEXTURE_X_Y, idx_list_down);

    // 设置向左轮廓动画
    animation_left_sketch.set_loop(true);
    animation_left_sketch.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_left_sketch.add_frame(tex_skeleton_sketch, SKELETON_TEXTURE_X_Y, idx_list_left);

    // 设置向右轮廓动画
    animation_right_sketch.set_loop(true);
    animation_right_sketch.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_right_sketch.add_frame(tex_skeleton_sketch, SKELETON_TEXTURE_X_Y, idx_list_right);


    // 设置属性
    max_hp            = skeleton_template.hp;
    max_speed         = skeleton_template.speed;
    damage            = skeleton_template.damage;
    reward_ratio      = skeleton_template.reward_ratio;
    recover_interval  = skeleton_template.recover_interval;
    recover_range     = skeleton_template.recover_range;
    recover_intensity = skeleton_template.recover_intensity;

    size = SKELETON_SIZE;

    hp    = max_hp;
    speed = max_speed;
}


// GoblinEnemy
GoblinEnemy::GoblinEnemy()
{
    // 数据模板
    static ConfigManager::EnemyTemplate& goblin_template = ConfigManager::Instance().goblin_template;

    // 获取纹理池
    static const ResourcesManager::TexturePool& texture_pool = ResourcesManager::Instance().get_texture_pool();

    // 获取纹理
    static SDL_Texture* tex_goblin        = texture_pool.find(ResID::Tex_Goblin)->second;
    static SDL_Texture* tex_goblin_sketch = texture_pool.find(ResID::Tex_GoblinSketch)->second;

    // 动画帧序列索引
    static const std::vector<int> idx_list_up    = GOBLIN_ANIMATION_IDX_LIST_UP;
    static const std::vector<int> idx_list_down  = GOBLIN_ANIMATION_IDX_LIST_DOWN;
    static const std::vector<int> idx_list_left  = GOBLIN_ANIMATION_IDX_LIST_LEFT;
    static const std::vector<int> idx_list_right = GOBLIN_ANIMATION_IDX_LIST_RIGHT;


    // 设置向上动画
    animation_up.set_loop(true);
    animation_up.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_up.add_frame(tex_goblin, GOBLIN_TEXTURE_X_Y, idx_list_up);

    // 设置向下动画
    animation_down.set_loop(true);
    animation_down.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_down.add_frame(tex_goblin, GOBLIN_TEXTURE_X_Y, idx_list_down);

    // 设置向左动画
    animation_left.set_loop(true);
    animation_left.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_left.add_frame(tex_goblin, GOBLIN_TEXTURE_X_Y, idx_list_left);

    // 设置向右动画
    animation_right.set_loop(true);
    animation_right.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_right.add_frame(tex_goblin, GOBLIN_TEXTURE_X_Y, idx_list_right);

    // 设置向上轮廓动画
    animation_up_sketch.set_loop(true);
    animation_up_sketch.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_up_sketch.add_frame(tex_goblin_sketch, GOBLIN_TEXTURE_X_Y, idx_list_up);

    // 设置向下轮廓动画
    animation_down_sketch.set_loop(true);
    animation_down_sketch.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_down_sketch.add_frame(tex_goblin_sketch, GOBLIN_TEXTURE_X_Y, idx_list_down);

    // 设置向左轮廓动画
    animation_left_sketch.set_loop(true);
    animation_left_sketch.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_left_sketch.add_frame(tex_goblin_sketch, GOBLIN_TEXTURE_X_Y, idx_list_left);

    // 设置向右轮廓动画
    animation_right_sketch.set_loop(true);
    animation_right_sketch.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_right_sketch.add_frame(tex_goblin_sketch, GOBLIN_TEXTURE_X_Y, idx_list_right);


    // 设置属性
    max_hp            = goblin_template.hp;
    max_speed         = goblin_template.speed;
    damage            = goblin_template.damage;
    reward_ratio      = goblin_template.reward_ratio;
    recover_interval  = goblin_template.recover_interval;
    recover_range     = goblin_template.recover_range;
    recover_intensity = goblin_template.recover_intensity;

    size = GOBLIN_SIZE;

    hp    = max_hp;
    speed = max_speed;
}


// GoblinPriestEnemy
GoblinPriestEnemy::GoblinPriestEnemy()
{
    // 数据模板
    static ConfigManager::EnemyTemplate& goblin_priest_template = ConfigManager::Instance().goblin_priest_template;

    // 获取纹理池
    static const ResourcesManager::TexturePool& texture_pool = ResourcesManager::Instance().get_texture_pool();

    // 获取纹理
    static SDL_Texture* tex_goblin_priest        = texture_pool.find(ResID::Tex_GoblinPriest)->second;
    static SDL_Texture* tex_goblin_priest_sketch = texture_pool.find(ResID::Tex_GoblinPriestSketch)->second;

    // 动画帧序列索引
    static const std::vector<int> idx_list_up    = GOBLIN_PRIEST_ANIMATION_IDX_LIST_UP;
    static const std::vector<int> idx_list_down  = GOBLIN_PRIEST_ANIMATION_IDX_LIST_DOWN;
    static const std::vector<int> idx_list_left  = GOBLIN_PRIEST_ANIMATION_IDX_LIST_LEFT;
    static const std::vector<int> idx_list_right = GOBLIN_PRIEST_ANIMATION_IDX_LIST_RIGHT;


    // 设置向上动画
    animation_up.set_loop(true);
    animation_up.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_up.add_frame(tex_goblin_priest, GOBLIN_PRIEST_TEXTURE_X_Y, idx_list_up);

    // 设置向下动画
    animation_down.set_loop(true);
    animation_down.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_down.add_frame(tex_goblin_priest, GOBLIN_PRIEST_TEXTURE_X_Y, idx_list_down);

    // 设置向左动画
    animation_left.set_loop(true);
    animation_left.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_left.add_frame(tex_goblin_priest, GOBLIN_PRIEST_TEXTURE_X_Y, idx_list_left);

    // 设置向右动画
    animation_right.set_loop(true);
    animation_right.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_right.add_frame(tex_goblin_priest, GOBLIN_PRIEST_TEXTURE_X_Y, idx_list_right);

    // 设置向上轮廓动画
    animation_up_sketch.set_loop(true);
    animation_up_sketch.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_up_sketch.add_frame(tex_goblin_priest_sketch, GOBLIN_PRIEST_TEXTURE_X_Y, idx_list_up);

    // 设置向下轮廓动画
    animation_down_sketch.set_loop(true);
    animation_down_sketch.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_down_sketch.add_frame(tex_goblin_priest_sketch, GOBLIN_PRIEST_TEXTURE_X_Y, idx_list_down);

    // 设置向左轮廓动画
    animation_left_sketch.set_loop(true);
    animation_left_sketch.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_left_sketch.add_frame(tex_goblin_priest_sketch, GOBLIN_PRIEST_TEXTURE_X_Y, idx_list_left);

    // 设置向右轮廓动画
    animation_right_sketch.set_loop(true);
    animation_right_sketch.set_interval(ANIMATION_FRAME_INTERVAL);
    animation_right_sketch.add_frame(tex_goblin_priest_sketch, GOBLIN_PRIEST_TEXTURE_X_Y, idx_list_right);


    // 设置属性
    max_hp            = goblin_priest_template.hp;
    max_speed         = goblin_priest_template.speed;
    damage            = goblin_priest_template.damage;
    reward_ratio      = goblin_priest_template.reward_ratio;
    recover_interval  = goblin_priest_template.recover_interval;
    recover_range     = goblin_priest_template.recover_range;
    recover_intensity = goblin_priest_template.recover_intensity;

    size = GOBLIN_PRIEST_SIZE;

    hp    = max_hp;
    speed = max_speed;

    timer_skill.set_wait_time(recover_interval);
}
