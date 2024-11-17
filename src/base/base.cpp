
// base.cpp

#include "base.h"
#include <cmath>

// Vector2 类

Vector2::Vector2(float x, float y)
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

Vector2
Vector2::operator*(float f) const
{
    return Vector2(vx * f, vy * f);
}

void
Vector2::operator*=(float f)
{
    vx *= f;
    vy *= f;
}

Vector2
Vector2::operator/(float f) const
{
    return Vector2(vx / f, vy / f);
}

void
Vector2::operator/=(float f)
{
    vx /= f;
    vy /= f;
}

float
Vector2::module() const
{
    return sqrt(vx * vx + vy * vy);
}

Vector2
Vector2::unit() const
{
    float m = module();

    if(m == 0)
    {
        return Vector2(0, 0);
    }
    else
    {
        return Vector2(vx / m, vy / m);
    }
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
Timer::set_wait_time(float t)
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
Timer::on_update(float dt)
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

// Atlas 类

void
Atlas::load(LPCTSTR path_template, int num)
{
    if(image_list) delete[] image_list;
    image_list = new IMAGE[num];
    image_num  = num;

    TCHAR path_file[MAX_PATH];
    for(int i = 1; i <= num; i++)
    {
        // 生成文件路径，加载图片
        _stprintf_s(path_file, path_template, i);
        loadimage(&image_list[i], path_file);
    }
}

void
Atlas::clear()
{
    if(image_list) delete[] image_list;
    image_num = 0;
}

int
Atlas::get_size() const
{
    return image_num;
}

IMAGE*
Atlas::get_image(int idx)
{
    if(idx < 0 || idx >= image_num)
    {
        return nullptr;
    }
    else
    {
        return &image_list[idx];
    }
}

// Animation 类

Animation::Animation()
{
    // lambda 表达式
    std::function<void()> f = [&]() {
        idx_frame++;
        if(idx_frame >= frame_num)
        {
            idx_frame = is_loop ? 0 : frame_num - 1;
            if(!is_loop && on_finished)
            {
                on_finished();
            }
        }
    };

    timer.set_one_shot(false); // 默认设置为循环播放
    timer.set_on_timeout(f);   // 设置回调函数
}

Animation::~Animation()
{
    clear_frame();
}

// 重置
void
Animation::reset()
{
    timer.restart();
    idx_frame = 0;
}

// 设置渲染中心
void
Animation::set_anchor_mode(AnchorMode mode)
{
    anchor_mode = mode;
}

// 设置位置
void
Animation::set_position(const Vector2& pos)
{
    position = pos;
}

// 设置是否循环播放
void
Animation::set_loop(bool flag)
{
    is_loop = flag;
}

// 设置帧间隔
void
Animation::set_interval(float interval)
{
    timer.set_wait_time(interval);
}

// 设置动画结束回调
void
Animation::set_on_finished(std::function<void()> f)
{
    on_finished = f;
}

// 添加帧，利用一个图片切割成多个帧
void
Animation::add_frame(IMAGE* img, int num, bool is_backward)
{
    clear_frame();
    frame_list = new Frame[num];
    frame_num  = num;

    int wide = img->getwidth();
    int high = img->getheight();

    int wide_frame = wide / num;
    if(is_backward)
    {
        int tali = 0;
        for(int i = num - 1; i >= 0; i--)
        {
            Rect rect_src = { i * wide_frame, 0, wide_frame, high };

            frame_list[tali++] = { rect_src, img };
        }
    }
    else
    {
        int tali = 0;
        for(int i = 0; i < num; i++)
        {
            Rect rect_src = { i * wide_frame, 0, wide_frame, high };

            frame_list[tali++] = { rect_src, img };
        }
    }
}

void
Animation::add_frame(Atlas* atlas, bool is_backward)
{
    clear_frame();

    if(is_backward)
    {
        int tali = 0;
        for(int i = atlas->get_size() - 1; i >= 0; i--)
        {
            IMAGE* img   = atlas->get_image(i);
            int    img_w = img->getwidth();
            int    img_h = img->getheight();

            Rect rect_src = { 0, 0, img_w, img_h };

            frame_list[tali++] = { rect_src, img };
        }
    }
    else
    {
        int tali = 0;
        for(int i = 0; i < atlas->get_size(); i++)
        {
            IMAGE* img   = atlas->get_image(i);
            int    img_w = img->getwidth();
            int    img_h = img->getheight();

            Rect rect_src = { 0, 0, img_w, img_h };

            frame_list[tali++] = { rect_src, img };
        }
    }
}

void
Animation::clear_frame()
{
    if(frame_list) delete[] frame_list;
    frame_num = 0;
}

// 更新
void
Animation::on_update(float dt)
{
    timer.on_update(dt);
}

// 渲染
void
Animation::on_render()
{
    const Frame& frame = frame_list[idx_frame];

    Rect rect_dst;
    rect_dst.w = frame.rect_src.w;
    rect_dst.h = frame.rect_src.h;
    if(anchor_mode == AnchorMode::TopLeft)
    {
        rect_dst.x = (int)position.vx;
        rect_dst.y = (int)position.vy;
    }
    else if(anchor_mode == AnchorMode::Centered)
    {
        rect_dst.x = (int)(position.vx - frame.rect_src.w / 2);
        rect_dst.y = (int)(position.vy - frame.rect_src.h / 2);
    }
    else if(anchor_mode == AnchorMode::BottomCentered)
    {
        rect_dst.x = (int)(position.vx - frame.rect_src.w / 2);
        rect_dst.y = (int)(position.vy - frame.rect_src.h);
    }

    putimage_a(frame.img, rect_dst, frame.rect_src);
}
