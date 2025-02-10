// 线程相关的封装

#pragma once
#include <cstdint>
#include <thread>
#include <functional>
#include <memory>
#include <string>
#include "src/util/noncopyable.h"

    // #include <Windows.h>
    #include <pthread.h>


namespace shs {

class Thread: NonCopyable {
public:
    using ptr = std::shared_ptr<Thread>;
    Thread(std::function<void()> cb, const std::string& name);
    ~Thread();

    int64_t getId() const { return m_id; }
    const std::string& getName() const { return m_name; }
    
    // 等待线程执行完成
    void join();

    // 获取当前的线程指针
    static Thread* GetThis();

    // 获取当前的线程名称
    static const std::string& GetName();

    // 设置当前线程名称
    static void SetName(const std::string& name);


private:

    // 线程执行函数

    // static DWORD WINAPI run(LPVOID arg);
    static void* run(void* arg);


    // 线程id
    int64_t m_id = -1;
    // 线程结构
    // std::thread m_thread;

    // HANDLE m_thread = nullptr;
    pthread_t m_thread = 0;


    // 线程执行函数
    std::function<void()> m_cb;
    // 线程名称
    std::string m_name;
};

}