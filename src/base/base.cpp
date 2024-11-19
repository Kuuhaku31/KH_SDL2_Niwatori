
// base.cpp

#include "base.h"

#include <cmath>

// Vector2 类

Vector2::Vector2(double x, double y)
    : vx(x)
    , vy(y)
{
}

Vector2
Vector2::operator+(const Vector2& v) const
{
    return Vector2(vx + v.vx, vy + v.vy);
}

void
Vector2::operator+=(const Vector2& v)
{
    vx += v.vx;
    vy += v.vy;
}

Vector2
Vector2::operator-(const Vector2& v) const
{
    return Vector2(vx - v.vx, vy - v.vy);
}

void
Vector2::operator-=(const Vector2& v)
{
    vx -= v.vx;
    vy -= v.vy;
}

double
Vector2::operator*(const Vector2& v) const
{
    return vx * v.vx + vy * v.vy;
}

Vector2
Vector2::operator*(double f) const
{
    return Vector2(vx * f, vy * f);
}

void
Vector2::operator*=(double f)
{
    vx *= f;
    vy *= f;
}

Vector2
Vector2::operator/(double f) const
{
    return Vector2(vx / f, vy / f);
}

void
Vector2::operator/=(double f)
{
    vx /= f;
    vy /= f;
}

bool
Vector2::operator==(const Vector2& v) const
{
    return vx == v.vx && vy == v.vy;
}

bool
Vector2::operator!=(const Vector2& v) const
{
    return vx != v.vx || vy != v.vy;
}

bool
Vector2::operator>(const Vector2& v) const
{
    return module() > v.module();
}

bool
Vector2::operator<(const Vector2& v) const
{
    return module() < v.module();
}

double
Vector2::module() const
{
    return sqrt(vx * vx + vy * vy);
}

Vector2
Vector2::unit() const
{
    double m = module();

    if(m == 0)
    {
        return Vector2(0, 0);
    }
    else
    {
        return Vector2(vx / m, vy / m);
    }
}

bool
Vector2::approx_zero() const
{
    return module() < 1e-4;
}

// Timer 类

// 设置等待时间
void
Timer::restart()
{
    pass_time = 0;
    shotted   = false;
}

// 设置等待时间
void
Timer::set_wait_time(double t)
{
    wait_time = t;
}

// 设置是否为一次性触发
void
Timer::set_one_shot(bool flag)
{
    one_shot = flag;
}

// 设置超时回调函数
void
Timer::set_on_timeout(std::function<void()> f)
{
    on_timeout = f;
}

// 暂停计时器
void
Timer::pause()
{
    paused = true;
}

// 恢复计时器
void
Timer::resume()
{
    paused = false;
}

// 更新计时器
void
Timer::on_update(double dt)
{
    // 如果计时器暂停，则不更新
    if(paused)
    {
        return;
    }

    // 更新计时器
    pass_time += dt;

    // 如果计时器超时
    if(pass_time >= wait_time)
    {
        // 如果计时器为一次性触发，则不再触发
        bool can_shot = (!one_shot || (one_shot && !shotted));
        shotted       = true;

        // 如果可以触发超时回调函数
        if(can_shot && on_timeout)
        {
            on_timeout();
        }

        // 重置计时器
        pass_time -= wait_time;
    }
}

// 立即触发
void
Timer::shot()
{
    shotted = true;
    on_timeout();
}

// Shape 类

Shape::~Shape()
{
    Shape_reset();
}

void
Shape::Shape_reset(int w, int h, int v)
{
    if(shape_buffer) delete[] shape_buffer;

    if(w > 0 && h > 0)
    {
        shape_buffer = new int[w * h];
        shape_wide   = w;
        shape_high   = h;
        shape_long   = w * h;

        for(int i = 0; i < shape_long; i++)
        {
            shape_buffer[i] = v;
        }
    }
    else
    {
        shape_buffer = nullptr;
        shape_wide   = 0;
        shape_high   = 0;
        shape_long   = 0;
    }
}

void
Shape::Shape_reset(const int* b, int w, int h)
{
    if(shape_buffer) delete[] shape_buffer;

    if(!b || 0 >= w || 0 >= h)
    {
        shape_buffer = nullptr;
        shape_wide   = 0;
        shape_high   = 0;
        shape_long   = 0;
    }
    else
    {
        shape_buffer = new int[w * h];
        shape_wide   = w;
        shape_high   = h;
        shape_long   = w * h;

        for(int i = 0; i < shape_long; i++)
        {
            shape_buffer[i] = b[i];
        }
    }
}

void
Shape::Shape_merge(Shape* s, std::function<void(int&, int&)> f, int dx, int dy)
{
    int* b1 = shape_buffer;
    int* b2 = s->shape_buffer;

    int A_start = 0;
    int A_skip  = 0;
    int B_start = 0;
    int B_skip  = 0;
    int wide    = s->shape_wide;
    int high    = s->shape_high;

    int R = dx + s->shape_wide - shape_wide;
    int L = dx;
    int T = dy;
    int B = dy + s->shape_high - shape_high;

    if(R > 0)
    {
        wide -= R;
        B_skip += R;
    }
    if(R < 0)
    {
        A_skip -= R;
    }
    if(L > 0)
    {
        A_skip += L;
        A_start += L;
    }
    if(L < 0)
    {
        wide += L;
        B_skip -= L;
        B_start -= L;
    }
    if(B > 0)
    {
        high -= B;
    }
    if(T > 0)
    {
        A_start += shape_wide * T;
    }
    if(T < 0)
    {
        high += T;
        B_start -= s->shape_wide * T;
    }

    for(int i = 0; i < high; i++)
    {
        for(int j = 0; j < wide; j++)
        {
            f(b1[A_start++], b2[B_start++]);
        }
        A_start += A_skip;
        B_start += B_skip;
    }
}

int
Shape::Shape_wide() const
{
    return shape_wide;
}

int
Shape::Shape_high() const
{
    return shape_high;
}

int
Shape::Shape_long() const
{
    return shape_long;
}

int*
Shape::Shape_buffer() const
{
    return shape_buffer;
}

int
Shape::Shape_in(int n) const
{
    if(n >= 0 && n < shape_long)
    {
        return shape_buffer[n];
    }
    else
    {
        return 0;
    }
}

int
Shape::Shape_in(int x, int y) const
{
    return Shape_in(y * shape_wide + x);
}
