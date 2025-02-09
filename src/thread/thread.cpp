#include "thread.h"
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
#ifdef _WIN32
    HANDLE rt = CreateThread(nullptr, 0, &Thread::run, this, 0, nullptr);
    if(!rt) {
        SHS_LOG_ERROR(g_logger) << "pthread_create thread fail, error=" << GetLastError() << " name=" << name;
        throw std::logic_error("pthread_create error");
    }
    m_thread = rt;
#else
    int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);    // 创建线程并执行 run 函数
    if(rt) {
        SHS_LOG_ERROR(g_logger) << "pthread_create thread fail, rt=" << rt << " name=" << name;
        throw std::logic_error("pthread_create error");
    }
#endif
}

Thread::~Thread() {
#ifdef _WIN32
    if(m_thread) {
        CloseHandle(m_thread);  // 关闭句柄，但线程会继续执行直到结束
    }
#else
    if(m_thread) {
        pthread_detach(m_thread);
    }
#endif
}

void Thread::join() {
#ifdef _WIN32
    // std::cout << "m_thread=" << m_thread << std::endl;
    if(m_thread) {
        WaitForSingleObject(m_thread, INFINITE);
        // std::cout << "m_thread=" << m_thread << std::endl;
        DWORD exitCode;
        
        if(!GetExitCodeThread(m_thread, &exitCode)) {
            SHS_LOG_ERROR(g_logger) << "WaitForSingleObject thread fail, exitCode=" << exitCode << " name=" << m_name;
            throw std::logic_error("WaitForSingleObject error");
        }
        CloseHandle(m_thread);
        m_thread = nullptr;
    }

#else
    if(m_thread) {
        int rt = pthread_join(m_thread, nullptr);
        if(rt) {
            SHS_LOG_ERROR(g_logger) << "pthread_join thread fail, rt=" << rt << " name=" << m_name;
            throw std::logic_error("pthread_join error");
        }
        m_thread = 0;
    }
#endif
}

#ifdef _WIN32
DWORD WINAPI Thread::run(LPVOID arg) {
    Thread* thread = static_cast<Thread*>(arg);
    t_thread = thread;
    t_thread_name = thread->m_name;
    thread->m_id = shs::GetThreadIdBySyscall();

    HRESULT hr = SetThreadDescription(GetCurrentThread(), std::wstring(thread->m_name.begin(), thread->m_name.end()).c_str());
    if (FAILED(hr))
    {
        SHS_LOG_ERROR(g_logger) << "SetThreadDescription thread fail" << " name=" << thread->m_name;
        return 1;

    }

    std::function<void()> cb;
    cb.swap(thread->m_cb);      // 不仅转移了可调用对象的所有权，还将 thread->m_cb 置空

    cb();
    return 0;

}
#else
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
#endif

}