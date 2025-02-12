#include <iostream>
#include <filesystem>
#include <chrono>
#include <thread>
#include "src/util/util.h"
#include "src/_log/log.h"
#include "src/config/config.h"
namespace fs = std::filesystem;

shs::Logger::ptr g_logger = SHS_LOG_NAME("file_monitor");

/**
 * @brief 监测文件大小变化
 * @param filepath 文件路径
 * @param interval 检查间隔时间（秒）
 */
void monitor_file_size(const std::string& filepath, int interval = 1, int loop_count = 10) {
    fs::path path(filepath);
    std::uintmax_t last_size = 0;
    auto last_time = std::chrono::steady_clock::now();

    while (loop_count--) {
        // 检查文件是否存在
        if (!fs::exists(path)) {
            SHS_LOG_INFO(g_logger) << "文件已被删除或不存在: " << filepath;
            break;
        }

        // 获取文件大小
        std::uintmax_t current_size = fs::file_size(path);
        auto current_time = std::chrono::steady_clock::now();

        // 计算文件大小变化率（字节/秒）
        auto time_diff = std::chrono::duration_cast<std::chrono::seconds>(current_time - last_time).count();
        if (time_diff > 0) {
            double rate = static_cast<double>(current_size - last_size) / time_diff;

            // 输出文件大小和变化率，单位为MB
            double current_size_mb = static_cast<double>(current_size) / (1024 * 1024);
            double rate_mb_per_sec = rate / (1024 * 1024); // 转换为MB/秒

            SHS_LOG_INFO(g_logger) << "文件大小: " << current_size_mb << " MB, 变化率: " << rate_mb_per_sec << " MB/秒";
        }

        // 更新上一次的文件大小和时间
        last_size = current_size;
        last_time = current_time;

        // 等待指定间隔时间
        std::this_thread::sleep_for(std::chrono::seconds(interval));
    }
}

int main(int argc, char** argv) {
    if(argc <= 1) {
        std::cout << "Usage: ./file_monitor MutexType" << std::endl;
        return 0;
    }
    // std::cout << argv[1] << std::endl;
    // return 0;

    YAML::Node root = YAML::LoadFile(shs::chooseByOs("D:\\repositories\\simple_http_server\\bin\\conf\\file_monitor.yml", "/home/wsl/repositories/simple_http_server/bin/conf/file_monitor.yml"));
    shs::Config::LoadFromYaml(root);
    std::string filepath = shs::chooseByOs("D:\\repositories\\simple_http_server\\bin\\mutex.txt", "/home/wsl/repositories/simple_http_server/bin/mutex.txt"); // 要监测的文件路径

    // std::string filepath = "test.txt";
    int interval = 1; // 检查间隔时间（秒）

    SHS_LOG_INFO(g_logger) << "===========================================================================================================";
    SHS_LOG_INFO(g_logger) << "MutexType = " << argv[1] << ":";
    SHS_LOG_INFO(g_logger) << "开始监测文件: " << filepath;
    monitor_file_size(filepath, interval);
    SHS_LOG_INFO(g_logger) << "===========================================================================================================";
    return 0;
}
