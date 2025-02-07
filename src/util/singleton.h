#pragma once
#include <memory>

namespace shs {

namespace {

template<class T, class X, int N>
T& GetInstanceX() {
    static T v;
    return v;
}

template<class T, class X, int N>
std::shared_ptr<T> GetInstancePtr() {
    static std::shared_ptr<T> v(new T);
    return v;
}


}

// T 类型
// X 为了创造多个实例对应的Tag
// N 同一个Tag创造多个实例索引
template<class T, class X = void, int N=0>
class SingleTon {
public:
    static T* GetInstance() {
        static T v;
        return &v;
        //return &GetInstanceX<T, X, N>();
    }
};


template<class T, class X = void, int N=0>
class SingleTonPtr {
public:
    static std::shared_ptr<T> GetInstance() {
        static auto v = std::make_shared<T>();
        return v;
        //return GetInstancePtr<T, X, N>();
    }
};
}