
// test.h

#pragma once

template<typename Tem>
class Manager
{
public:
    static Tem& Instance()
    {
        if(manager == nullptr)
        {
            manager = new Tem();
        }
        return *manager;
    }

protected:
    Manager()                          = default;
    ~Manager()                         = default;
    Manager(const Manager&)            = delete; // 禁用拷贝构造函数
    Manager& operator=(const Manager&) = delete; // 禁用赋值构造函数

private:
    static Tem* manager;
};

template<typename Tem>
Tem* Manager<Tem>::manager = nullptr;
