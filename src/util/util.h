#pragma once
#include <cstdint>
#include <ctime>
#include <string>
#include <sstream>
#include <typeinfo>
#include <cxxabi.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/syscall.h>
#include <unistd.h>
#endif

namespace shs {

// 跨平台安全的 localtime 实现
bool safe_localtime(const time_t* time, struct tm* result);

// 通过操作系统api返回当前线程的ID
uint64_t GetThreadIdBySyscall();

// 通过C++标准库返回当前线程的ID
std::string GetThreadId();

// 返回当前协程的ID
uint64_t GetFiberId();

template<class T>
const char* TypeToName() {
    static const char* s_name = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr);
    return s_name;
}

std::string chooseByOs(const std::string& win, const std::string& linux);

}
