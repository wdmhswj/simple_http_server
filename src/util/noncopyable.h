#pragma once 

namespace shs {

// 信号量
class NonCopyable {
public:
    NonCopyable() = default;
    ~NonCopyable() = default;
    NonCopyable(const NonCopyable& o) = delete;
    NonCopyable& operator=(const NonCopyable& o) = delete;
private:
};

}