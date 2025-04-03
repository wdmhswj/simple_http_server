#pragma once

#include <memory>
#include <functional>
#include <ucontext.h>
#include <cstdint>

namespace shs {

// 协程调度类的声明
class Scheduler;

// 协程类
class Fiber: public std::enable_shared_from_this<Fiber> {

friend class Scheduler; // 有元类

public:
    using ptr = std::shared_ptr<Fiber>;

    // 协程状态
    enum State {
        INIT,   // 初始状态
        HOLD,   // 暂停状态
        EXEC,   // 执行状态
        TERM,   // 结束状态
        READY,  // 可执行状态
        EXCEPT, // 异常状态
    };

    // 构造函数：子协程的构造
    Fiber(std::function<void()> cb, std::size_t stack_size = 0, bool use_caller = false);

    ~Fiber();

    // 重置协程执行函数，并设置状态
    void reset(std::function<void()> cb);

    // 将当前协程切换到运行状态
    void swapIn();

    // 将当前协程切换到后台
    void swapOut();

    // 将当前线程切换到执行状态，执行的为当前线程的主协程
    void call();

    // 将当前线程切换到后台，返回到线程的主协程
    void back();

    // 获取协程id
    uint64_t getId() const { return m_id; }

    // 获取协程状态
    State getState() const { return m_state; }
    
    // 设置当前线程的运行协程
    static void SetThis(Fiber* f);

    // 返回当前所在的协程
    static Fiber::ptr GetThis();

    // 将当前协程切换到后台，并设置为ready状态
    static void YieldToReady();
 
    // 将当前协程切换到后台，并设置为hold状态
    static void YieldToHold();

    // 返回当前协程的总数目
    static uint64_t TotalFibers();

    // 协程执行函数：执行完成返回到线程主协程
    static void MainFunc();

    // 协程执行函数：执行完成返回到线程调度协程
    static void CallerMainFunc();

    // 获取当前协程的id
    static uint64_t GetFiberId();
private:

    // 无参构造函数：每个线程第一个协程(主协程)的构造
    Fiber();

    // 协程id
    uint64_t m_id = 0;
    // 协程运行栈大小
    uint32_t m_stack_size = 0;
    // 协程状态
    State m_state = INIT;
    // 协程上下文
    ucontext_t m_ctx;
    // 协程运行栈指针
    void* m_stack = nullptr;
    // 协程运行函数
    std::function<void()> m_cb;
};

}