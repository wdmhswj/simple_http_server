#include "util.h"
#include <pthread.h>
#include <thread>

namespace shs {

// 跨平台安全的 localtime 实现
bool safe_localtime(const time_t* time, struct tm* result) {
#ifdef _WIN32
    // Windows 平台使用 localtime_s
    return localtime_s(result, time) == 0;
#else
    // POSIX 平台使用 localtime_r
    return localtime_r(time, result) != nullptr;
#endif
}


// 通过操作系统api返回当前线程的ID
uint64_t GetThreadIdBySyscall() {
#ifdef _WIN32
    return static_cast<uint64_t>(GetCurrentThreadId());
#else
    return static_cast<uint64_t>(syscall(SYS_gettid));
#endif
}

// 通过C++标准库返回当前线程的ID
std::string GetThreadId() {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    return ss.str();
}


// 返回当前协程的ID
uint64_t GetFiberId() {
    return 0;
}



}
