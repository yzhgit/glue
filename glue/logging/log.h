//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/logging/logger.h"

#include <stdexcept>

#ifdef GL_NO_LOG
    #define CHECK(x)                                                           \
        if (!(x)) {                                                            \
            throw std::runtime_error("Check failed: " #x);                     \
        }
#else
    #define CHECK(x)                                                           \
        if (!(x)) {                                                            \
            LOG_FATAL("", "Check failed: " #x);                                \
            throw std::runtime_error("Check failed: " #x);                     \
        }
#endif

#define CHECK_EQ(x, y) CHECK((x) == (y))
#define CHECK_NE(x, y) CHECK((x) != (y))
#define CHECK_LT(x, y) CHECK((x) < (y))
#define CHECK_LE(x, y) CHECK((x) <= (y))
#define CHECK_GT(x, y) CHECK((x) > (y))
#define CHECK_GE(x, y) CHECK((x) >= (y))

constexpr const char *Basename(const char *fname, int offset) {
    return offset == 0 || fname[offset - 1] == '/' || fname[offset - 1] == '\\'
               ? fname + offset
               : Basename(fname, offset - 1);
}

#ifdef GL_NO_LOG
    #define LOG_DEBUG(name, fmt, ...)
    #define LOG_INFO(name, fmt, ...)
    #define LOG_WARN(name, fmt, ...)
    #define LOG_ERROR(name, fmt, ...)
    #define LOG_FATAL(name, fmt, ...)
#else
    #define LOG_DEBUG(name, fmt, ...)                                          \
        glue::log::LoggerRegistry()::get().getLogger(name)->log(               \
            glue::log::LogLevel::DEBUG,                                        \
            Basename(__FILE__, sizeof(__FILE__) - 1), __LINE__, fmt,           \
            ##__VA_ARGS__)

    #define LOG_INFO(name, fmt, ...)                                           \
        glue::log::LoggerRegistry()::get().getLogger(name)->log(               \
            glue::log::LogLevel::INFO,                                         \
            Basename(__FILE__, sizeof(__FILE__) - 1), __LINE__, fmt,           \
            ##__VA_ARGS__)

    #define LOG_WARN(name, fmt, ...)                                           \
        glue::log::LoggerRegistry()::get().getLogger(name)->log(               \
            glue::log::LogLevel::WARN,                                         \
            Basename(__FILE__, sizeof(__FILE__) - 1), __LINE__, fmt,           \
            ##__VA_ARGS__)

    #define LOG_ERROR(name, fmt, ...)                                          \
        glue::log::LoggerRegistry()::get().getLogger(name)->log(               \
            glue::log::LogLevel::ERROR,                                        \
            Basename(__FILE__, sizeof(__FILE__) - 1), __LINE__, fmt,           \
            ##__VA_ARGS__)

    #define LOG_FATAL(name, fmt, ...)                                          \
        glue::log::LoggerRegistry()::get().getLogger(name)->log(               \
            glue::log::LogLevel::FATAL,                                        \
            Basename(__FILE__, sizeof(__FILE__) - 1), __LINE__, fmt,           \
            ##__VA_ARGS__)
#endif
