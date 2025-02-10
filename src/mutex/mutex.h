#pragma once
#include "src/util/noncopyable.h"
#include <pthread.h>
#include <cstdint>

#include <semaphore.h>

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

}