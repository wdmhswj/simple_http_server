#include "util.h"
#include <pthread.h>
#include <thread>
#include <execinfo.h>
#include "src/_log/log.h"

namespace shs {

static shs::Logger::ptr g_logger = SHS_LOG_NAME("system");

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

std::string chooseByOs(const std::string& win, const std::string& linux) {
    #ifdef PLATFORM_WINDOWS
        return win;
    #elif defined(PLATFORM_LINUX)
        return linux;
    #endif
        return "";

}

// 获取当前的调用栈
void Backtrace(std::vector<std::string>& bt, int size, int skip) {
    void** buffer = (void**)malloc((sizeof(void*)*size));
    size_t s = backtrace(buffer, size); 

    char** strings = backtrace_symbols(buffer, s);
    if(strings == nullptr) {
        SHS_LOG_ERROR(g_logger) << "backtrace_synbols error";
        free(buffer);
    }

    for(size_t i=skip; i<s; ++i) {
        bt.push_back(strings[i]);
    }

    free(strings);
    free(buffer);
}

// 获取当前栈信息的字符串
std::string BacktraceToString(int size, int skip, const std::string& prefix) {
    std::vector<std::string> bt;
    Backtrace(bt, size, skip);
    std::stringstream ss;
    for(size_t i=0; i<bt.size(); ++i) {
        ss << prefix << bt[i] << std::endl;
    }
    return ss.str();
}
}
