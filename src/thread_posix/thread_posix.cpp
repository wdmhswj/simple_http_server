#include "thread_posix.h"
#include "src/_log/log.h"


namespace shs {

static thread_local Thread* t_thread = nullptr;             // 借助 thread_local 实现线程内的唯一性
static thread_local std::string t_thread_name = "UNKNOW";

static shs::Logger::ptr g_logger = SHS_LOG_NAME("system");

Thread* Thread::GetThis() {
    return t_thread;
}


// 获取当前的线程名称
const std::string& Thread::GetName() {
    return t_thread_name;
}

// 设置当前线程名称
void Thread::SetName(const std::string& name) {
    if(name.empty())
        return;
    if(t_thread) 
        t_thread->m_name = name;            // 私有成员不能直接访问？
    t_thread_name = name;
    
}

Thread::Thread(std::function<void()> cb, const std::string& name)
    :m_cb(cb)
    ,m_name(name) {
    if(name.empty())
        m_name = "UNKNOW";

    int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);    // 创建线程并执行 run 函数
    if(rt) {
        SHS_LOG_ERROR(g_logger) << "pthread_create thread fail, rt=" << rt << " name=" << name;
        throw std::logic_error("pthread_create error");
    }

}

Thread::~Thread() {

    if(m_thread) {
        pthread_detach(m_thread);
    }

}

void Thread::join() {

    if(m_thread) {
        int rt = pthread_join(m_thread, nullptr);
        if(rt) {
            SHS_LOG_ERROR(g_logger) << "pthread_join thread fail, rt=" << rt << " name=" << m_name;
            throw std::logic_error("pthread_join error");
        }
        m_thread = 0;
    }

}


void* Thread::run(void* arg) {
    Thread* thread = static_cast<Thread*>(arg); // arg 指向Thread对象
    t_thread = thread;
    t_thread_name = thread->m_name;             // ?
    thread->m_id = shs::GetThreadIdBySyscall();

    pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());

    std::function<void()> cb;
    cb.swap(thread->m_cb);      // 不仅转移了可调用对象的所有权，还将 thread->m_cb 置空

    cb();
    return nullptr;
}

}