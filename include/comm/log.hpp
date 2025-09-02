#pragma once

#include <cstdarg>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <fstream>

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <boost/stacktrace.hpp>
#include <csignal>
#include <sstream>

#define LOG_INIT(module) async_log_init(module)
#define LOG_TRACE(...) SPDLOG_TRACE(__VA_ARGS__)
#define LOG_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
#define LOG_INFO(...) SPDLOG_INFO(__VA_ARGS__)
#define LOG_WARN(...) SPDLOG_WARN(__VA_ARGS__)
#define LOG_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)
#define LOG_CRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__)
#define LOG_FLUSH() async_log_close()

static inline void crashHandler(int signum) {
    LOG_CRITICAL("Received signal {}", signum);

    // 使用 boost::stacktrace 获取调用栈
    LOG_CRITICAL("=== Crash Stack Trace ===");
    boost::stacktrace::stacktrace st = boost::stacktrace::stacktrace();

    std::stringstream ss;
    ss << boost::stacktrace::stacktrace();
    LOG_INFO("\n{}", ss.str());
    spdlog::shutdown();
    spdlog::default_logger()->flush();
    // 恢复默认信号处理并重新触发信号
    std::signal(signum, SIG_DFL);
    std::raise(signum);
}

static inline void async_log_init(const std::string &module) {
    // 1️ 初始化线程池，只执行一次
    static bool initialized = false;
    if(initialized) {
        return;
    }

    initialized = true;
    spdlog::init_thread_pool(8192, 1); // 队列大小 8192, 后台线程 1

    // 2️ 创建 sinks：控制台 + 文件（支持轮转）
    std::vector<spdlog::sink_ptr> sinks;

    // 控制台输出（带颜色）
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

    // 文件输出，5MB 文件，保留 3 个轮转文件
    std::string log_file =  std::string(getenv("HOME")) + "/logs/" + module + "_async.log";
    sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(log_file, 5 * 1024 * 1024, 3));

    // 3️ 创建异步 logger
    auto async_logger = std::make_shared<spdlog::async_logger>(
        module + "_async_logger",    // logger 名称
        sinks.begin(), sinks.end(),  // sinks 集合
        spdlog::thread_pool(),       // 使用线程池
        spdlog::async_overflow_policy::block // 队列满时阻塞
    );

    async_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e][%^%l%$][%s:%#][%t] %v");
    async_logger->set_level(spdlog::level::info);

    spdlog::set_error_handler([](const std::string& msg) {
        std::cerr << "spdlog error handler: " << msg << std::endl;
    });

    spdlog::set_default_logger(async_logger);
    //崩溃日志
    std::signal(SIGSEGV, crashHandler);  // 段错误
    std::signal(SIGABRT, crashHandler);  // 异常终止
    std::signal(SIGFPE, crashHandler);   // 浮点异常
    std::signal(SIGILL, crashHandler);   // 非法指令
}

static inline void async_log_close() {

    spdlog::default_logger()->flush();
    spdlog::shutdown();
}

