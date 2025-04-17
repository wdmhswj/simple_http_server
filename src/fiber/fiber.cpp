#include "fiber.h"
#include "src/config/config.h"
#include "src/macro.h"
#include "src/_log/log.h"
#include "src/scheduler/scheduler.h"
#include <atomic>

namespace shs {

// 日志器
static Logger::ptr g_logger = SHS_LOG_NAME("system");

// 静态变量：控制线程id、线程计数
static std::atomic<uint64_t> s_fiber_id{0};
static std::atomic<uint64_t> s_fiber_count{0};

// 线程局部变量
static thread_local Fiber* t_fiber = nullptr;   // 当前正在执行的协程
static thread_local Fiber::ptr t_threadFiber = nullptr; // 线程主协程

static ConfigVar<uint32_t>::ptr g_fiber_stack_size = Config::Lookup<uint32_t>(128*1024, "fiber.stack_size", "fiber stack size");




// 协程栈空间分配器
class MallocStackAllocator {
public:
    static void* Alloc(std::size_t size) {
        return malloc(size);
    }
    static void DeAlloc(void* vp, std::size_t size) {
        return free(vp);
    }
};

using StackAllocator = MallocStackAllocator;


// 获取当前协程的id
uint64_t Fiber::GetFiberId() {
    if(t_fiber) {
        return t_fiber->getId();
    }
    return 0;
}

// 主协程构造函数
Fiber::Fiber() {
    m_state = EXEC;
    SetThis(this);

    if(getcontext(&m_ctx)) {
        SHS_ASSERT2(false, "getcontext");
    }

    ++s_fiber_count;

    SHS_LOG_DEBUG(g_logger) << "Fiber::Fiber main";
}

// 构造函数：子协程的构造
Fiber::Fiber(std::function<void()> cb, size_t stack_size = 0, bool use_caller = false)
    :m_id(++s_fiber_id)
    ,m_cb(cb) {
    ++s_fiber_count;
    m_stack_size = stack_size ? stack_size : g_fiber_stack_size->getValue();    // 如果未提供协程栈大小，设置为默认的配置

    m_stack = StackAllocator::Alloc(m_stack_size);  // 分配栈空间
    if(getcontext(&m_ctx)) {
        SHS_ASSERT2(false, "getcontext");
    }
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stack_size;

    if(!use_caller) {
        makecontext(&m_ctx, &Fiber::MainFunc, 0);
    } else {
        makecontext(&m_ctx, &Fiber::CallerMainFunc, 0);
    }

    SHS_LOG_DEBUG(g_logger) << "Fiber::Fiber id=" << m_id;
}

Fiber::~Fiber() {
    --s_fiber_count;
    if(m_stack) {
        SHS_ASSERT(m_state == TERM
                || m_state == EXCEPT
                || m_state == INIT);
        StackAllocator::DeAlloc(m_stack, m_stack_size);
    } else {
        SHS_ASSERT(!m_cb);
        SHS_ASSERT(m_state == EXEC);

        Fiber* cur = t_fiber;
        if(cur == this) {
            SetThis(nullptr);
        }
    }
    SHS_LOG_DEBUG(g_logger) << "Fiber::~Fiber id=" << m_id << " total=" << s_fiber_count;
}

void Fiber::SetThis(Fiber* f) {
    t_fiber = f;
}

Fiber::ptr Fiber::GetThis() {
    if(t_fiber) {
        return t_fiber->shared_from_this();
    }
    Fiber::ptr main_fiber = std::make_shared<Fiber>();
    SHS_ASSERT(t_fiber == main_fiber.get());
    t_threadFiber = main_fiber;
    return t_fiber->shared_from_this();

}

// 重置协程执行函数，并设置线程状态
void Fiber::reset(std::function<void()> cb) {
    SHS_ASSERT(m_stack);
    SHS_ASSERT(m_state == TERM
            || m_state == EXCEPT
            || m_state == INIT);
    m_cb = cb;
    if(getcontext(&m_ctx)) {
        SHS_ASSERT2(false, "getcontext");
    }

    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stack_size;

    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    m_state = INIT;
}


// 将当前线程切换到执行状态
void Fiber::call() {
    SetThis(this);
    m_state = EXEC;
    if(swapcontext(&t_threadFiber->m_ctx, &m_ctx)) {
        SHS_ASSERT2(false, "swapcontext");
    }
}

// 将当前线程切换到后台
void Fiber::back() {
    SetThis(t_threadFiber.get());
    if(swapcontext(&m_ctx, &t_threadFiber->m_ctx)) {
        SHS_ASSERT2(false, "swapcontext");
    }
}


// 将当前协程切换到运行状态
void Fiber::swapIn() {
    SetThis(this);
    SHS_ASSERT(m_state != EXEC);
    m_state = EXEC;
    if(swapcontext(&Scheduler::GetMainFiber()->m_ctx, &m_ctx)) {
        SHS_ASSERT2(false, "swapcontext");
    }
}

//切换到后台执行
void Fiber::swapOut() {
    SetThis(Scheduler::GetMainFiber());
    if(swapcontext(&m_ctx, &Scheduler::GetMainFiber()->m_ctx)) {
        SHS_ASSERT2(false, "swapcontext");
    }
}

//协程切换到后台，并且设置为Ready状态
void Fiber::YieldToReady() {
    Fiber::ptr cur = GetThis();
    SHS_ASSERT(cur->m_state == EXEC);
    cur->m_state = READY;
    cur->swapOut();
}

//协程切换到后台，并且设置为Hold状态
void Fiber::YieldToHold() {
    Fiber::ptr cur = GetThis();
    SHS_ASSERT(cur->m_state == EXEC);
    //cur->m_state = HOLD;
    cur->swapOut();
}

//总协程数
uint64_t Fiber::TotalFibers() {
    return s_fiber_count;
}

void Fiber::MainFunc() {
    Fiber::ptr cur = GetThis();
    SHS_ASSERT(cur);
    try {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    } catch (std::exception& ex) {
        cur->m_state = EXCEPT;
        SHS_LOG_ERROR(g_logger) << "Fiber Except: " << ex.what()
            << " fiber_id=" << cur->getId()
            << std::endl
            << shs::BacktraceToString();
    } catch (...) {
        cur->m_state = EXCEPT;
        SHS_LOG_ERROR(g_logger) << "Fiber Except"
            << " fiber_id=" << cur->getId()
            << std::endl
            << shs::BacktraceToString();
    }

    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->swapOut();

    SHS_ASSERT2(false, "never reach fiber_id=" + std::to_string(raw_ptr->getId()));
}

void Fiber::CallerMainFunc() {
    Fiber::ptr cur = GetThis();
    SHS_ASSERT(cur);
    try {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    } catch (std::exception& ex) {
        cur->m_state = EXCEPT;
        SHS_LOG_ERROR(g_logger) << "Fiber Except: " << ex.what()
            << " fiber_id=" << cur->getId()
            << std::endl
            << shs::BacktraceToString();
    } catch (...) {
        cur->m_state = EXCEPT;
        SHS_LOG_ERROR(g_logger) << "Fiber Except"
            << " fiber_id=" << cur->getId()
            << std::endl
            << shs::BacktraceToString();
    }

    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->back();
    SHS_ASSERT2(false, "never reach fiber_id=" + std::to_string(raw_ptr->getId()));

}

}