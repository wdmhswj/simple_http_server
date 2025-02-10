#include "mutex.h"
#include <stdexcept>
#include <iostream>

namespace shs {

Semaphore::Semaphore(uint32_t count) {
    if(sem_init(&m_semaphore, 0, count)) {
        throw std::logic_error("sem_init error");
    }
}
Semaphore::~Semaphore() {
    if(sem_destroy(&m_semaphore)) {
        std::cerr << "sem_destroy error" << std::endl;
    }
}
void Semaphore::wait() {
    if(sem_wait(&m_semaphore)) {
        throw std::logic_error("sem_wait error");
    }
}
void Semaphore::notify() {
    if(sem_post(&m_semaphore)) {
        throw std::logic_error("sem_post error");
    }
}

Mutex::Mutex() {
    if(pthread_mutex_init(&m_mutex, nullptr)) {
        throw std::logic_error("pthread_mutex_init error");
    }
}
Mutex::~Mutex() {
    if(pthread_mutex_destroy(&m_mutex)) {
        std::cerr << "pthread_mutex_destroy error:  EBUSY - The mutex is currenty locked" << std::endl;
    }
}

void Mutex::lock() {
    if(pthread_mutex_lock(&m_mutex)) {
        throw std::logic_error("pthread_mutex_lock error");
    }
}
void Mutex::unlock() {
    if(pthread_mutex_unlock(&m_mutex)) {
        throw std::logic_error("pthread_mutex_unlock error");
    }
}

RWMutex::RWMutex() {
    if(pthread_rwlock_init(&m_lock, nullptr)) {
        throw std::logic_error("pthread_rwlock_init error");
    }
}
RWMutex::~RWMutex() {
    if(pthread_rwlock_destroy(&m_lock)) {
        std::cerr << "pthread_rwlock_destroy error" << std::endl;
    }
}
void RWMutex::rdlock() {
    if(pthread_rwlock_rdlock(&m_lock)) {
        throw std::logic_error("pthread_rwlock_rdlock error");
    }
}
void RWMutex::wrlock() {
    if(pthread_rwlock_wrlock(&m_lock)) {
        throw std::logic_error("pthread_rwlock_wrlock error");
    }
}
void RWMutex::unlock() {
    if(pthread_rwlock_unlock(&m_lock)) {
        throw std::logic_error("pthread_rwlock_unlock error");
    }
}
}