#include "src/fiber/fiber.h"
#include "src/_log/log.h"
#include "src/thread/thread.h"

shs::Logger::ptr g_logger = SHS_LOG_ROOT();

void run_in_fiber() {
    SHS_LOG_INFO(g_logger) << "run_in_fiber begin";
    shs::Fiber::YieldToHold();
    SHS_LOG_INFO(g_logger) << "run_in_fiber end";
    shs::Fiber::YieldToHold();
}

// void test_fiber() {
//     SHS_LOG_INFO(g_logger) << "main begin -1";
//     {
//         shs::Fiber::GetThis();
//         SHS_LOG_INFO(g_logger) << "main begin";
//         shs::Fiber::ptr fiber(new shs::Fiber(run_in_fiber));
//         fiber->swapIn();
//         SHS_LOG_INFO(g_logger) << "main after swapIn";
//         fiber->swapIn();
//         SHS_LOG_INFO(g_logger) << "main after end";
//         fiber->swapIn();
//     }
//     SHS_LOG_INFO(g_logger) << "main after end2";
// }

int main(int argc, char** argv) {
    shs::Fiber::GetThis();
    SHS_LOG_INFO(g_logger) << "main begin";
    shs::Fiber::ptr fiber(new shs::Fiber(run_in_fiber));
    fiber->swapIn();
    SHS_LOG_INFO(g_logger) << "main after swapIn";
    fiber->swapIn();
    SHS_LOG_INFO(g_logger) << "main after end";
    
    return 0;
}