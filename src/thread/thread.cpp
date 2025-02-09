#include "thread.h"
#include "log.h"


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


}