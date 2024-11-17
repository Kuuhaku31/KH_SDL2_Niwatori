
// base.h

#pragma once

#include "util.h"

#include <functional>
#include <graphics.h>


class Vector2
{
public:
    float vx = 0;
    float vy = 0;

public:
    Vector2() = default;
    Vector2(float x, float y);
    ~Vector2() = default;

public:
    Vector2 operator+(const Vector2& v) const;
    void    operator+=(const Vector2& v);
    Vector2 operator-(const Vector2& v) const;
    void    operator-=(const Vector2& v);
    Vector2 operator*(float f) const;
    void    operator*=(float f);
    Vector2 operator/(float f) const;
    void    operator/=(float f);

    float   module() const;
    Vector2 unit() const;
};


class Timer
{
public:
    Timer()  = default;
    ~Timer() = default;

public:
    void restart();
    void set_wait_time(float t);
    void set_one_shot(bool flag);
    void set_on_timeout(std::function<void()> f);
    void pause();  // 暂停计时器
    void resume(); // 恢复计时器
    void on_update(float dt);
    void shot(); // 立即触发

private:
    float pass_time = 0;     // 计时器已经过去的时间
    float wait_time = 0;     // 计时器等待的时间
    bool  paused    = false; // 计时器是否暂停
    bool  shotted   = false; // 计时器是否已经触发过一次
    bool  one_shot  = false; // 计时器是否为一次性触发

    // 超时回调函数
    std::function<void()> on_timeout = nullptr;
};


// 最基础的图形类，用四个字节的数组表示一个矩阵，每个字节表示一个像素点的颜色
class Shape
{
public:
    Shape() = default;
    ~Shape(); // **记得释放内存**

public:
    // 重新设置形状
    void Shape_reset(int = 0, int = 0, int = 0);
    void Shape_reset(const int*, int, int);

    // 合并两个形状
    void Shape_merge(Shape*, std::function<void(int&, int&)> f, int dx = 0, int dy = 0);

    // 获取形状的宽高、缓冲区大小
    int Shape_wide() const;
    int Shape_high() const;
    int Shape_long() const;

    int* Shape_buffer() const;

    // 获取形状某个点的值
    int Shape_in(int) const;
    int Shape_in(int, int) const;

    // 绘制圆形
    void Shape_draw_point(int, int, int = 0);
    void Shape_draw_line(int, int, int, int, int = 0);
    void Shape_draw_rectangle(int, int, int, int, int = 0);
    void Shape_draw_circle(int, int, int, int = 0);

    // 复制形状
    void Shape_copy(Shape*);
    // 设置所有单位的值
    void Shape_clear(int = 0);

protected:
    int* shape_buffer = nullptr;
    int  shape_wide   = 0;
    int  shape_high   = 0;
    int  shape_long   = 0;
};


class Atlas
{
public:
    Atlas()  = default;
    ~Atlas() = default;

public:
    // 加载图集
    void   load(LPCTSTR path_template, int num);
    void   clear();
    int    get_size() const;
    IMAGE* get_image(int idx);

private:
    IMAGE* image_list = nullptr;
    int    image_num  = 0;
};


class Animation
{
public:
    enum class AnchorMode
    {
        TopLeft,
        Centered,
        BottomCentered,
    };

public:
    Animation();
    ~Animation();

public:
    void reset();                                  // 重置
    void set_anchor_mode(AnchorMode mode);         // 设置渲染中心
    void set_position(const Vector2& pos);         // 设置位置
    void set_loop(bool flag);                      // 设置是否循环播放
    void set_interval(float interval);             // 设置帧间隔
    void set_on_finished(std::function<void()> f); // 设置动画结束回调

    // 添加帧，利用一个图片切割成多个帧
    void add_frame(IMAGE* img, int num, bool is_backward = false);

    // 添加帧，利用图集添加帧，is_backward 为 true 时，逆序添加
    void add_frame(Atlas* atlas, bool is_backward = false);

    // 清空帧
    void clear_frame();

    void on_update(float dt); // 更新
    void on_render();         // 渲染

private:
    // 帧结构体
    struct Frame
    {
        Rect   rect_src = { 0, 0, 0, 0 };
        IMAGE* img      = nullptr;
    };

private:
    Frame*  frame_list;
    int     frame_num = 0;
    size_t  idx_frame = 0;
    Timer   timer;
    bool    is_loop = true;
    Vector2 position;

    std::function<void()> on_finished;

    AnchorMode anchor_mode = AnchorMode::Centered;
};
