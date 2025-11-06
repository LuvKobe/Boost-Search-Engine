#pragma once

#include <iostream>
#include <cstdio>
#include <string>
#include <cstdarg>
#include <chrono>
#include <ctime>
#include <unistd.h> // getpid

// 日志等级（避免与系统/宏冲突，建议改名更安全，如 LOG_DEBUG）
#define DEBUG 0
#define NORMAL 1
#define WARNING 2
#define ERROR 3
#define FATAL 4

inline const char* to_levelstr(int level)
{
    switch (level)
    {
    case DEBUG:   return "DEBUG";
    case NORMAL:  return "NORMAL";
    case WARNING: return "WARNING";
    case ERROR:   return "ERROR";
    case FATAL:   return "FATAL";
    default:      return "UNKNOWN";
    }
}

// 可选：GCC/Clang 下启用 printf 风格格式检查
#if defined(__GNUC__) || defined(__clang__)
__attribute__((format(printf, 2, 3)))
#endif
inline void logMsg(int level, const char* format, ...)  // 可变参数列表
{
    // [等级] [YYYY-MM-DD HH:MM:SS] [pid: N] message

    // 获取当前时间（到秒）
    auto now   = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    struct tm local_time;
    localtime_r(&now_c, &local_time);

    char time_str[20]; // "YYYY-MM-DD HH:MM:SS"
    std::snprintf(time_str, sizeof(time_str),
                  "%04d-%02d-%02d %02d:%02d:%02d",
                  1900 + local_time.tm_year,
                  1 + local_time.tm_mon,
                  local_time.tm_mday,
                  local_time.tm_hour,
                  local_time.tm_min,
                  local_time.tm_sec);

    constexpr size_t NUM = 1024;

    char logprefix[NUM];
    std::snprintf(logprefix, sizeof(logprefix),
                  "[%s] [%s] [pid: %d] ",
                  to_levelstr(level), time_str, getpid());

    char logcontent[NUM];
    va_list arg;
    va_start(arg, format);
    std::vsnprintf(logcontent, sizeof(logcontent), format, arg);
    va_end(arg);  // ★ 必须有

    // 级别区分输出位置（可选：WARNING 及以上走 stderr）
    if (level >= WARNING) {
        std::cerr << logprefix << logcontent << std::endl;
    } else {
        std::cout << logprefix << logcontent << std::endl;
    }
}
