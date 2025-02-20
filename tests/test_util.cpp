#include "src/macro.h"

shs::Logger::ptr g_logger = SHS_LOG_ROOT();

void test_assert() {
    SHS_LOG_INFO(g_logger) << shs::BacktraceToString(10);
    // SHS_ASSERT2(0 == 1, "abcdef xx");
}

int main(int argc, char** argv) {
    test_assert();

    // int arr[] = {1,3,5,7,9,11};

    // SHS_LOG_INFO(g_logger) << sylar::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 0);
    // SHS_LOG_INFO(g_logger) << sylar::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 1);
    // SHS_LOG_INFO(g_logger) << sylar::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 4);
    // SHS_LOG_INFO(g_logger) << sylar::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 13);
    // SHS_ASSERT(sylar::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 0) == -1);
    // SHS_ASSERT(sylar::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 1) == 0);
    // SHS_ASSERT(sylar::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 2) == -2);
    // SHS_ASSERT(sylar::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 3) == 1);
    // SHS_ASSERT(sylar::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 4) == -3);
    // SHS_ASSERT(sylar::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 5) == 2);
    // SHS_ASSERT(sylar::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 6) == -4);
    // SHS_ASSERT(sylar::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 7) == 3);
    // SHS_ASSERT(sylar::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 8) == -5);
    // SHS_ASSERT(sylar::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 9) == 4);
    // SHS_ASSERT(sylar::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 10) == -6);
    // SHS_ASSERT(sylar::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 11) == 5);
    // SHS_ASSERT(sylar::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 12) == -7);
    return 0;
}