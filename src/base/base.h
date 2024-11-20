
// base.h

#pragma once

#include <functional>


class Vector2
{
public:
    double vx = 0;
    double vy = 0;

public:
    Vector2() = default;
    Vector2(double x, double y);
    ~Vector2() = default;

public:
    Vector2 operator+(const Vector2& v) const;
    void    operator+=(const Vector2& v);
    Vector2 operator-(const Vector2& v) const;
    void    operator-=(const Vector2& v);
    double  operator*(const Vector2& v) const;
    Vector2 operator*(double f) const;
    void    operator*=(double f);
    Vector2 operator/(double f) const;
    void    operator/=(double f);

    bool operator==(const Vector2& v) const;
    bool operator!=(const Vector2& v) const;
    bool operator>(const Vector2& v) const;
    bool operator<(const Vector2& v) const;

    double  module() const;
    Vector2 unit() const;

    bool approx_zero() const;
};


class Timer
{
public:
    Timer()  = default;
    ~Timer() = default;

public:
    void restart();
    void set_wait_time(double t);
    void set_one_shot(bool flag);
    void set_on_timeout(std::function<void()> f);
    void pause();  // 暂停计时器
    void resume(); // 恢复计时器
    void on_update(double dt);
    void shot(); // 立即触发

private:
    double pass_time = 0;     // 计时器已经过去的时间
    double wait_time = 0;     // 计时器等待的时间
    bool   paused    = false; // 计时器是否暂停
    bool   shotted   = false; // 计时器是否已经触发过一次
    bool   one_shot  = false; // 计时器是否为一次性触发

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

enum class Facing
{
    Up,
    Down,
    Left,
    Right
};
