/*
除log模块宏之外的常用宏的封装
*/

#pragma once
#include "src/_log/log.h"
#include <cassert>
#include "src/util/util.h"

// 断言宏封装
#define SHS_ASSERT(x) \
    if(!(x)) {  \
        SHS_LOG_ERROR(SHS_LOG_ROOT()) << "ASSERTION: " #x \
            << "\nbacktrace:\n" \
            << shs::BacktraceToString(100, 2, "    ");  \
        assert(x);  \
    }

// 断言宏封装
#define SHS_ASSERT2(x, w) \
    if(!(x)) {  \
        SHS_LOG_ERROR(SHS_LOG_ROOT()) << "ASSERTION: " #x \
            << "\n" << w    \
            << "\nbacktrace:\n" \
            << shs::BacktraceToString(100, 2, "    ");  \
        assert(x);  \
    }
