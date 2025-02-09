#include "../src/_log/log.h"
#include "../src/thread/thread.h"
#include <unistd.h>
#include <thread>
#include <chrono>


shs::Logger::ptr g_logger = SHS_LOG_ROOT();

int count = 0;
//shs::RWMutex s_mutex;
// shs::Mutex s_mutex;

void fun1() {
    SHS_LOG_INFO(g_logger) << "name: " << shs::Thread::GetName()
                             << " this.name: " << shs::Thread::GetThis()->getName()
                             << " id: " << shs::GetThreadIdBySyscall()
                             << " this.id: " << shs::Thread::GetThis()->getId();

    std::this_thread::sleep_for(std::chrono::seconds(30));
    // for(int i = 0; i < 100000; ++i) {
    //     //shs::RWMutex::WriteLock lock(s_mutex);
    //     shs::Mutex::Lock lock(s_mutex);
    //     ++count;
    // }
}

void fun2() {
    while(true) {
        SHS_LOG_INFO(g_logger) << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    }
}

void fun3() {
    while(true) {
        SHS_LOG_INFO(g_logger) << "========================================";
    }
}

int main(int argc, char** argv) {
    SHS_LOG_INFO(g_logger) << "thread test begin";
    // YAML::Node root = YAML::LoadFile("/home/shs/test/shs/bin/conf/log2.yml");
    // shs::Config::LoadFromYaml(root);

    std::vector<shs::Thread::ptr> thrs;
    for(int i = 0; i < 5; ++i) {
        shs::Thread::ptr thr(new shs::Thread(&fun1, "name_" + std::to_string(i)));
        //shs::Thread::ptr thr2(new shs::Thread(&fun3, "name_" + std::to_string(i * 2 + 1)));
        thrs.push_back(thr);
        //thrs.push_back(thr2);
    }

    for(size_t i = 0; i < 5; ++i) {
        thrs[i]->join();
    }
    SHS_LOG_INFO(g_logger) << "thread test end";
    // SHS_LOG_INFO(g_logger) << "count=" << count;

    return 0;
}