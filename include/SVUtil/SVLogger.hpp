#pragma once

#include <iostream>
#include <string>
#include <cstdarg>


// 枚举类，用于表示不同的日志级别
enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
};

// 单例模式，一个类只能有一个实例，并提供全局访问点来访问该实例
class SVLogger {
public:
    // 当第一次调用 getInstance() 函数时，会创建唯一的实例并将其存储在静态局部变量中
    // 静态局部变量的生命周期从程序第一次进入该函数时开始，直到程序结束，因此实例会一直存在
    // 当再次调用 getInstance() 函数时，由于静态局部变量已经创建并存储了实例，不会再执行构造函数，而是返回之前创建的实例的地址
    static SVLogger& getInstance() {  // 通过静态成员函数 getInstance 来获取唯一的实例
        static SVLogger instance;
        return instance;
    }

    void setLogLevel(LogLevel level) {  // 设置全局日志级别
        logLevel_ = level;
    }

    void log_debug(const char* format, ...) {
        if (logLevel_ == LogLevel::DEBUG){
            va_list args;
            va_start(args, format);
            vprintf(format, args);
            va_end(args);
        }
    }

    void log_info(const char* format, ...) {
        if (logLevel_ <= LogLevel::INFO){
            va_list args;
            va_start(args, format);
            vprintf(format, args);
            va_end(args);
        }
    }

    void log_warning(const char* format, ...) {
        if (logLevel_ <= LogLevel::WARNING){
            va_list args;
            va_start(args, format);
            vprintf(format, args);
            va_end(args);
        }
    }

    void log_error(const char* format, ...) {
        if (logLevel_ <= LogLevel::ERROR){
            va_list args;
            va_start(args, format);
            vprintf(format, args);
            va_end(args);
        }
    }

    void log_fatal(const char* format, ...) {
        if (logLevel_ <= LogLevel::FATAL){
            va_list args;
            va_start(args, format);
            vprintf(format, args);
            va_end(args);
            std::exit(1);
        }
    }

private:
    LogLevel logLevel_ = LogLevel::INFO;

    SVLogger() = default;  // 私有构造函数，确保不能直接创建对象
    ~SVLogger() = default;
    SVLogger(const SVLogger&) = delete;  // 禁用拷贝构造函数
    SVLogger& operator=(const SVLogger&) = delete;  // 禁用赋值运算符
};


#define LOG_DEBUG(tag, msg, ...) SVLogger::getInstance().log_debug("[ DEBUG ] [ " tag " ] " msg "\n", ##__VA_ARGS__)
#define LOG_INFO(tag, msg, ...) SVLogger::getInstance().log_info("[ INFO ] [ " tag " ] " msg "\n", ##__VA_ARGS__)
#define LOG_WARNING(tag, msg, ...) SVLogger::getInstance().log_warning("[ WARNING ] [ " tag " ] " msg "\n", ##__VA_ARGS__)
#define LOG_ERROR(tag, msg, ...) SVLogger::getInstance().log_error("[ ERROR ] [ " tag " ] " msg "\n", ##__VA_ARGS__)
#define LOG_FATAL(tag, msg, ...) SVLogger::getInstance().log_fatal("[ FATAL ] [ " tag " ] " msg "\n", ##__VA_ARGS__)
