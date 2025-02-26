#include "src/_log/log.h"
#include "src/thread_posix/thread_posix.h"
#include <unistd.h>
// #include <thread>
#include <chrono>
#include "src/mutex/mutex.h"
#include "src/config/config.h"

shs::Logger::ptr g_logger = SHS_LOG_ROOT();

long long count = 0;
shs::RWMutex s_rwmutex;
shs::Mutex s_mutex;

void fun1() {
    SHS_LOG_INFO(g_logger) << "name: " << shs::Thread_posix::GetName()
                             << " this.name: " << shs::Thread_posix::GetThis()->getName()
                             << " id: " << shs::GetThreadIdBySyscall()
                             << " this.id: " << shs::Thread_posix::GetThis()->getId();

    // std::this_thread::sleep_for(std::chrono::seconds(1));
    for(long long i = 0; i < 1000000; ++i) {
        // shs::RWMutex::WriteLock lock(s_rwmutex);
        // shs::Mutex::Lock lock(s_mutex);
        ++count;
    }
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
    std::string yaml_path = RESOURCES_DIR "/conf/log2.yml";
    // YAML::Node root = YAML::LoadFile(shs::chooseByOs("D:\\repositories\\simple_http_server\\bin\\conf\\log2.yml", "~/repositories/simple_http_server/bin/conf/log2.yml"));
    YAML::Node root = YAML::LoadFile(yaml_path);
    shs::Config::LoadFromYaml(root);

    std::vector<shs::Thread_posix::ptr> thrs;
    for(int i = 0; i < 2; ++i) {
        // shs::Thread_posix::ptr thr(new shs::Thread_posix(&fun1, "name_" + std::to_string(i)));
        shs::Thread_posix::ptr thr1(new shs::Thread_posix(&fun2, "name_" + std::to_string(i * 2 + 1)));
        shs::Thread_posix::ptr thr2(new shs::Thread_posix(&fun3, "name_" + std::to_string(i * 2)));
        // thrs.push_back(thr);
        thrs.push_back(thr1);
        thrs.push_back(thr2);
    }

    for(size_t i = 0; i < thrs.size(); ++i) {
        thrs[i]->join();
    }
    SHS_LOG_INFO(g_logger) << "thread test end";
    SHS_LOG_INFO(g_logger) << "count=" << count;

    return 0;
}