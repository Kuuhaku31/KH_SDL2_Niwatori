
// game_manager.cpp

#include "game_manager.h"

#include "bullet_manager.h"
#include "coin_manager.h"
#include "config_manager.h"
#include "enemy_manager.h"
#include "player_manager.h"
#include "resources_manager.h"
#include "tower_manager.h"
#include "wave_manager.h"

#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"

int
GameManager::Run(int argc, char** argv)
{
    // debug
    TowerManager::Instance().Place_tower(TowerType::Archer, { 5, 0 });

    Uint64 last_counter = SDL_GetPerformanceCounter();   // 获取CPU计数器
    Uint64 counter_freq = SDL_GetPerformanceFrequency(); // 获取CPU频率
    // 事件循环
    while(is_running)
    {
        // 输入
        on_input();

        Uint64 current_counter = SDL_GetPerformanceCounter();

        double delta = (double)(current_counter - last_counter) / (double)counter_freq;
        last_counter = current_counter;
        if(delta < 1.0 / FPS) SDL_Delay((1.0 / FPS - delta) * 1000);


        // 更新
        on_update(delta);

        // 渲染
        on_render();
    }

    return 0;
}

GameManager::GameManager()
{
    // 检测初始化是否成功
    init_assert(!SDL_Init(SDL_INIT_EVERYTHING), (const char*)u8"SDL_Init failed!");
    init_assert(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG), (const char*)u8"IMG_Init failed!");
    init_assert(Mix_Init(MIX_INIT_MP3), (const char*)u8"Mix_Init failed!");
    init_assert(!TTF_Init(), (const char*)u8"TTF_Init failed!");

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048); // 打开音频设备

    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1"); // 设置输入法显示

    // 加载配置
    ConfigManager& config = ConfigManager::Instance();
    init_assert(config.map.Load("../config/map.csv"), (const char*)u8"加载游戏地图失败！");
    init_assert(config.Load_level_config("../config/level.json"), (const char*)u8"加载关卡配置失败！");
    init_assert(config.Load_game_config("../config/config.json"), (const char*)u8"加载游戏配置失败！");

    window = SDL_CreateWindow(
        config.basic_template.window_title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        config.basic_template.window_width,
        config.basic_template.window_height,
        SDL_WINDOW_SHOWN);
    init_assert(window, (const char*)u8"SDL_CreateWindow failed!");

    // 开启硬件加速、垂直同步、渲染目标纹理
    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE); // 创建渲染器
    init_assert(renderer, (const char*)u8"SDL_CreateRenderer failed!");

    // 加载资源
    init_assert(ResourcesManager::Instance().load_from_file(renderer), (const char*)u8"加载游资源置失败！");
    init_assert(generate_tile_map_texture(), (const char*)u8"生成地图纹理失败！");

    status_bar.Set_position(15, 15);

    place_panel   = new PlacePanel();
    upgrade_panel = new UpgradePanel();
}

GameManager::~GameManager()
{
    SDL_DestroyRenderer(renderer); // 销毁渲染器
    SDL_DestroyWindow(window);     // 销毁窗口

    Mix_CloseAudio(); // 关闭音频设备

    TTF_Quit(); // 退出TTF
    Mix_Quit(); // 退出Mixer
    IMG_Quit(); // 退出IMG
    SDL_Quit(); // 退出SDL
}

void
GameManager::init_assert(bool flag, const char* msg)
{
    if(flag) return;

    // 弹出错误提示框
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!", msg, window);
    exit(-1);
}

void
GameManager::on_input()
{
    static ConfigManager& config_manager = ConfigManager::Instance();

    static SDL_Point pos_center;
    static SDL_Point idx_tile_selected;

    // 输入处理
    while(SDL_PollEvent(&event))
    {
        PlayerManager::Instance().On_input(event);

        switch(event.type)
        {
            case SDL_QUIT:
            {
                is_running = false;
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                if(config_manager.is_game_over) break;

                if(get_cursor_idx_tile(idx_tile_selected, event.button.x, event.button.y))
                {
                    get_selected_tile_center_position(pos_center, idx_tile_selected);

                    if(check_home(idx_tile_selected))
                    {
                        upgrade_panel->Set_idx_tile(idx_tile_selected);
                        upgrade_panel->Set_center_pos(pos_center);
                        upgrade_panel->Make_visible();
                    }
                    else if(can_place_tower(idx_tile_selected))
                    {
                        place_panel->Set_idx_tile(idx_tile_selected);
                        place_panel->Set_center_pos(pos_center);
                        place_panel->Make_visible();
                    }
                }
                break;
            }
            default: break;
        }

        if(!config_manager.is_game_over)
        {
            place_panel->On_input(event);
            upgrade_panel->On_input(event);
        }
    }
}

void
GameManager::on_update(double delta_time) // 更新
{
    static ConfigManager& config_manager = ConfigManager::Instance();

    if(!config_manager.is_game_over)
    {
        PlayerManager::Instance().On_update(delta_time);
        WaveManager::Instance().On_update(delta_time);
        EnemyManager::Instance().On_update(delta_time);
        BulletManager::Instance().On_update(delta_time);
        TowerManager::Instance().On_update(delta_time);
        CoinManager::Instance().On_update(delta_time);

        status_bar.On_update(renderer);

        place_panel->On_update(renderer);
        upgrade_panel->On_update(renderer);
    }
}

void
GameManager::on_render()
{
    // 渲染
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // 设置渲染颜色
    SDL_RenderClear(renderer);                      // 清空渲染目标

    static ConfigManager& config   = ConfigManager::Instance();
    static SDL_Rect&      rect_dis = config.rect_tile_map;

    SDL_RenderCopy(renderer, tex_tile_map, nullptr, &rect_dis); // 渲染地图纹理

    EnemyManager::Instance().On_render(renderer);  // 渲染敌人
    BulletManager::Instance().On_render(renderer); // 渲染子弹
    TowerManager::Instance().On_render(renderer);  // 渲染塔
    CoinManager::Instance().On_render(renderer);   // 渲染金币
    PlayerManager::Instance().On_render(renderer); // 渲染玩家

    if(!config.is_game_over)
    {
        place_panel->On_render(renderer);
        upgrade_panel->On_render(renderer);
        status_bar.On_render(renderer); // 渲染状态栏
    }

    SDL_RenderPresent(renderer); // 显示渲染目标
}

// 生成地图纹理
bool
GameManager::generate_tile_map_texture()
{
    const Map&     map      = ConfigManager::Instance().map;
    const TileMap& tile_map = map.Get_tile_map();

    SDL_Rect&    rect_tile_map = ConfigManager::Instance().rect_tile_map;
    SDL_Texture& tex_tile_set  = *ResourcesManager::Instance().get_texture_pool().find(ResID::Tex_Tileset)->second;

    int tex_tile_set_width, tex_tile_set_height;
    SDL_QueryTexture(&tex_tile_set, nullptr, nullptr, &tex_tile_set_width, &tex_tile_set_height); // 获取纹理宽高

    int num_tile_single_line = (int)ceil((double)tex_tile_set_width / SIZE_TILE); // 计算单行瓦片数（向上取整）s

    int tex_tile_map_width  = (int)map.Get_map_width() * SIZE_TILE;  // 计算地图纹理宽度
    int tex_tile_map_height = (int)map.Get_map_height() * SIZE_TILE; // 计算地图纹理高度

    tex_tile_map = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888, // 像素格式
        SDL_TEXTUREACCESS_TARGET, // 纹理访问权限
        tex_tile_map_width,
        tex_tile_map_height);
    if(!tex_tile_map) return false;

    ConfigManager& config = ConfigManager::Instance();

    // 设置渲染目标位置
    rect_tile_map.x = 0; // (config.basic_template.window_width - tex_tile_map_width) >> 1;
    rect_tile_map.y = 0; //(config.basic_template.window_height - tex_tile_map_height) >> 1;
    rect_tile_map.w = tex_tile_map_width;
    rect_tile_map.h = tex_tile_map_height;

    SDL_SetTextureBlendMode(tex_tile_map, SDL_BLENDMODE_BLEND); // 设置混合模式
    SDL_SetRenderTarget(renderer, tex_tile_map);                // 设置渲染目标

    for(int y = 0; y < map.Get_map_height(); y++)
    {
        for(int x = 0; x < map.Get_map_width(); x++)
        {
            const Tile& tile = tile_map[y][x];

            const SDL_Rect& dst_rect = { x * SIZE_TILE, y * SIZE_TILE, SIZE_TILE, SIZE_TILE }; // 目标矩形

            SDL_Rect src_rect = {
                (tile.terrain % num_tile_single_line) * SIZE_TILE,
                (tile.terrain / num_tile_single_line) * SIZE_TILE,
                SIZE_TILE,
                SIZE_TILE
            }; // 源矩形

            SDL_RenderCopy(renderer, &tex_tile_set, &src_rect, &dst_rect); // 渲染瓦片

            // 如果有装饰
            if(tile.decoration >= 0)
            {
                src_rect = {
                    (tile.decoration % num_tile_single_line) * SIZE_TILE,
                    (tile.decoration / num_tile_single_line) * SIZE_TILE,
                    SIZE_TILE,
                    SIZE_TILE
                }; // 源矩形

                SDL_RenderCopy(renderer, &tex_tile_set, &src_rect, &dst_rect); // 渲染装饰
            }
        }
    }

    const SDL_Point& idx_home = map.Get_idx_home();
    const SDL_Rect&  dst_rect = { idx_home.x * SIZE_TILE, idx_home.y * SIZE_TILE, SIZE_TILE, SIZE_TILE };                        // 目标矩形
    SDL_RenderCopy(renderer, ResourcesManager::Instance().get_texture_pool().find(ResID::Tex_Home)->second, nullptr, &dst_rect); // 渲染家

    SDL_SetRenderTarget(renderer, nullptr); // 取消渲染目标

    return true;
}

bool
GameManager::get_cursor_idx_tile(SDL_Point& idx_tile_selected, int screen_x, int screen_y)
{
    static const Map& map = ConfigManager::Instance().map;

    static const SDL_Rect& rect_tile_map = ConfigManager::Instance().rect_tile_map;

    if(screen_x < rect_tile_map.x ||
       screen_x > rect_tile_map.x + rect_tile_map.w ||
       screen_y < rect_tile_map.y ||
       screen_y > rect_tile_map.y + rect_tile_map.h)
    {
        return false; // 如果鼠标不在地图范围内
    }

    // 计算鼠标所在瓦片索引，不允许超出地图范围
    idx_tile_selected.x = std::min((screen_x - rect_tile_map.x) / SIZE_TILE, (int)map.Get_map_width() - 1);
    idx_tile_selected.y = std::min((screen_y - rect_tile_map.y) / SIZE_TILE, (int)map.Get_map_height() - 1);

    return true;
}

bool
GameManager::can_place_tower(const SDL_Point& idx_tile_selected)
{
    static const Map& map = ConfigManager::Instance().map;

    const Tile& tile = map.Get_tile_map()[idx_tile_selected.y][idx_tile_selected.x];

    return (tile.decoration < 0 && tile.direction == Tile::Direction::None && !tile.has_tower);
}

bool
GameManager::check_home(const SDL_Point& idx_tile_selected)
{
    static const Map&       map      = ConfigManager::Instance().map;
    static const SDL_Point& idx_home = map.Get_idx_home();

    return (idx_tile_selected.x == idx_home.x && idx_tile_selected.y == idx_home.y);
}

void
GameManager::get_selected_tile_center_position(SDL_Point& center_pos, const SDL_Point& idx_tile_selected) const
{
    static const SDL_Rect& rect_tile_map = ConfigManager::Instance().rect_tile_map;

    center_pos.x = rect_tile_map.x + idx_tile_selected.x * SIZE_TILE + (SIZE_TILE >> 1);
    center_pos.y = rect_tile_map.y + idx_tile_selected.y * SIZE_TILE + (SIZE_TILE >> 1);
}
