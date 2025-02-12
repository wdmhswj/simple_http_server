#pragma once
#include "src/util/noncopyable.h"
#include <pthread.h>
#include <cstdint>

#include <semaphore.h>
#include <atomic>

namespace shs {

class Semaphore: NonCopyable {
public:
    Semaphore(uint32_t count = 0);
    ~Semaphore();
    void wait();
    void notify();
private:
    sem_t m_semaphore;
};

// 局部锁的模板实现
template<typename T>
struct ScopedLockImpl {
public:
    ScopedLockImpl(T& mutex): m_mutex(mutex) {
        m_mutex.lock();
        m_locked = true;
    }
    ~ScopedLockImpl() {
        unlock();
    }
    void lock() {
        if(!m_locked) {
            m_mutex.lock();
            m_locked = true;
        }
    }
    void unlock() {
        if(m_locked) {
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    // mutex
    T& m_mutex;     // 引用类型：因为mutex一般无法复制，引用成员变量需要在构造函数的初始化列表中初始化
    // 是否已经上锁
    bool m_locked;
};

// 局部读锁的模板实现
template<typename T>
struct ReadScopedLockImpl {
public:
    ReadScopedLockImpl(T& mutex): m_mutex(mutex) {
        m_mutex.rdlock();
        m_locked = true;
    }
    ~ReadScopedLockImpl() {
        unlock();
    }
    void lock() {
        if(!m_locked) {
            m_mutex.rdlock();
            m_locked = true;
        }
    }
    void unlock() {
        if(m_locked) {
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    // mutex
    T& m_mutex;
    // 是否已经上锁
    bool m_locked;
};



// 局部写锁的模板实现
template<typename T>
struct WriteScopedLockImpl {
public:
    WriteScopedLockImpl(T& mutex): m_mutex(mutex) {
        m_mutex.wrlock();
        m_locked = true;
    }
    ~WriteScopedLockImpl() {
        unlock();
    }
    void lock() {
        if(!m_locked) {
            m_mutex.wrlock();
            m_locked = true;
        }
    }
    void unlock() {
        if(m_locked) {
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    // mutex
    T& m_mutex;
    // 是否已经上锁
    bool m_locked;
};


// 互斥量
class Mutex: NonCopyable {
public:
    // 局部锁
    using Lock = ScopedLockImpl<Mutex>;
    Mutex();
    ~Mutex();
    void lock();
    void unlock();

private:
    // mutex
    pthread_mutex_t m_mutex;
};


// Null互斥量
class NullMutex: NonCopyable {
public:
    // 局部锁
    using Lock = ScopedLockImpl<NullMutex>;
    NullMutex() {}
    ~NullMutex() {}
    void lock() {}
    void unlock() {}

private:
    // mutex
    pthread_mutex_t m_mutex;
};

// 读写互斥量
class RWMutex: NonCopyable {
public:
    // 局部读锁
    using ReadLock = ReadScopedLockImpl<RWMutex>;
    // 局部写锁
    using WriteLock = WriteScopedLockImpl<RWMutex>;
    RWMutex();
    ~RWMutex();
    void rdlock();
    void wrlock();
    void unlock();

private:
    // mutex
    pthread_rwlock_t m_lock;
};
// 读写互斥量
class NullRWMutex: NonCopyable {
public:
    // 局部读锁
    using ReadLock = ReadScopedLockImpl<NullRWMutex>;
    // 局部写锁
    using WriteLock = WriteScopedLockImpl<NullRWMutex>;
    NullRWMutex() {}
    ~NullRWMutex() {}
    void rdlock() {}
    void wrlock() {}
    void unlock() {}

private:
    // mutex
    pthread_rwlock_t m_lock;
};

// 自旋锁
class SpinLock: NonCopyable {
public:
    using Lock = ScopedLockImpl<SpinLock>;

    SpinLock();
    ~SpinLock();

    void lock();
    void unlock();

private:
    pthread_spinlock_t m_mutex;
};

// 原子锁
class CASLock: NonCopyable {
public:
    using Lock = ScopedLockImpl<CASLock>;
    CASLock() {
        m_mutex.clear();
    }
    ~CASLock() {
    }
    void lock() noexcept
    {
        while (m_mutex.test_and_set(std::memory_order_acquire))
    #if defined(__cpp_lib_atomic_wait) && __cpp_lib_atomic_wait >= 201907L
            // C++ 20 起可以仅在 unlock 中通知后才获得锁，从而避免任何无效自旋
            // 注意，即使 wait 保证一定在值被更改后才返回，但锁定是在下一次执行条件时完成的
            m_.wait(true, std::memory_order_relaxed)
    #endif
                ;
    }
    bool try_lock() noexcept
    {
        return !m_mutex.test_and_set(std::memory_order_acquire);
    }
    void unlock() noexcept
    {
        m_mutex.clear(std::memory_order_release);
    #if defined(__cpp_lib_atomic_wait) && __cpp_lib_atomic_wait >= 201907L
        m_.notify_one();
    #endif
    }
private:
    std::atomic_flag m_mutex;
};
   
}