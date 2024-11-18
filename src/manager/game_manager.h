
#include "manager.h"

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"

#define WINDOW_TITLE (const char*)u8"SDL2 Window" // 窗口标题
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define FPS 60

class GameManager : public Manager<GameManager>
{
    friend class Manager<GameManager>;

public:
    int
    Run(int argc, char** argv)
    {
        Uint64 last_counter = SDL_GetPerformanceCounter();   // 获取CPU计数器
        Uint64 counter_freq = SDL_GetPerformanceFrequency(); // 获取CPU频率
        // 事件循环
        while(is_running)
        {
            while(SDL_PollEvent(&event))
            {
                on_input();
            }

            Uint64 current_counter = SDL_GetPerformanceCounter();

            double delta = (double)(current_counter - last_counter) / (double)counter_freq;
            last_counter = current_counter;
            if(delta < 1.0 / FPS) SDL_Delay((1.0 / FPS - delta) * 1000);


            // 更新
            on_update();

            // 渲染
            on_render();
        }

        return 0;
    }

protected:
    GameManager()
    {
        // 检测初始化是否成功
        init_assert(!SDL_Init(SDL_INIT_EVERYTHING), (const char*)u8"SDL_Init failed!");
        init_assert(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG), (const char*)u8"IMG_Init failed!");
        init_assert(Mix_Init(MIX_INIT_MP3), (const char*)u8"Mix_Init failed!");
        init_assert(!TTF_Init(), (const char*)u8"TTF_Init failed!");

        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048); // 打开音频设备

        SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1"); // 设置输入法显示

        window = SDL_CreateWindow(
            WINDOW_TITLE,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            SDL_WINDOW_SHOWN);
        init_assert(window, (const char*)u8"SDL_CreateWindow failed!");

        // 开启硬件加速、垂直同步、渲染目标纹理
        SDL_Renderer* renderer = SDL_CreateRenderer(
            window,
            -1,
            SDL_RENDERER_ACCELERATED |
                SDL_RENDERER_PRESENTVSYNC |
                SDL_RENDERER_TARGETTEXTURE); // 创建渲染器
        init_assert(renderer, (const char*)u8"SDL_CreateRenderer failed!");
    }

    ~GameManager()
    {
        SDL_DestroyRenderer(renderer); // 销毁渲染器
        SDL_DestroyWindow(window);     // 销毁窗口

        Mix_CloseAudio(); // 关闭音频设备

        TTF_Quit(); // 退出TTF
        Mix_Quit(); // 退出Mixer
        IMG_Quit(); // 退出IMG
        SDL_Quit(); // 退出SDL
    }

private:
    SDL_Window* window = nullptr;

    SDL_Event event;
    bool      is_running = true;

    SDL_Renderer* renderer = nullptr;

private:
    void
    init_assert(bool flag, const char* msg)
    {
        if(flag) return;

        // 弹出错误提示框
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!", msg, window);
        exit(-1);
    }

    void on_input()
    {
        // 输入处理
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                {
                    is_running = false;
                }
                break;
                default:
                {
                }
                break;
            }
        }
    }

    void on_update()
    {
        // 更新
    }

    void on_render()
    {
        // 渲染
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // 设置渲染颜色
        SDL_RenderClear(renderer);                      // 清空渲染目标


        SDL_RenderPresent(renderer); // 显示渲染目标
    }
};
