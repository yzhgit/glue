//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/logging/init.h"

#include <stdexcept>

constexpr const char *Basename(const char *fname, int offset) {
    return offset == 0 || fname[offset - 1] == '/' || fname[offset - 1] == '\\'
               ? fname + offset
               : Basename(fname, offset - 1);
}

#define __FILENAME__ Basename(__FILE__, sizeof(__FILE__) - 1)

#ifdef LOG_DISABLE_LOGGING
    #ifdef _MSC_VER
        #define IF_LOG(severity)                                               \
            __pragma(warning(push)) __pragma(warning(disable : 4127))
if (true) {
    ;
} else
    __pragma(warning(pop)) // conditional expression is constant
    #else
        #define IF_LOG(severity)                                               \
            if (true) {                                                        \
                ;                                                              \
            } else
    #endif
#else
    #define IF_LOG(severity)                                                   \
        if (!glue::log::get() || !glue::log::get()->checkSeverity(severity)) { \
            ;                                                                  \
        } else
#endif

#define LOG(severity)                                                          \
    IF_LOG(severity)                                                           \
    (*glue::log::get()) += glue::log::Record(severity, __LINE__, __FILENAME__)

#define LOGD LOG(glue::log::debug)
#define LOGI LOG(glue::log::info)
#define LOGW LOG(glue::log::warn)
#define LOGE LOG(glue::log::error)
#define LOGF LOG(glue::log::fatal)

#define LOG_IF(severity, condition)                                            \
    if (!(condition)) {                                                        \
        ;                                                                      \
    } else                                                                     \
        LOG(severity)

#define LOGD_IF(condition) LOG_IF(glue::log::debug, condition)
#define LOGI_IF(condition) LOG_IF(glue::log::info, condition)
#define LOGW_IF(condition) LOG_IF(glue::log::warn, condition)
#define LOGE_IF(condition) LOG_IF(glue::log::error, condition)
#define LOGF_IF(condition) LOG_IF(glue::log::fatal, condition)

#define LOG_FMT(severity, format, ...)                                         \
    IF_LOG(severity)                                                           \
    glue::log::get()->log(severity, __LINE__, __FILENAME__, format,            \
                          ##__VA_ARGS__)

#define LOGD_FMT(format, ...) LOG_FMT(glue::log::debug, format, ##__VA_ARGS__)
#define LOGI_FMT(format, ...) LOG_FMT(glue::log::info, format, ##__VA_ARGS__)
#define LOGW_FMT(format, ...) LOG_FMT(glue::log::warn, format, ##__VA_ARGS__)
#define LOGE_FMT(format, ...) LOG_FMT(glue::log::error, format, ##__VA_ARGS__)
#define LOGF_FMT(format, ...) LOG_FMT(glue::log::fatal, format, ##__VA_ARGS__)

#define LOG_FMT_IF(severity, condition, format, ...)                           \
    if (!(condition)) {                                                        \
        ;                                                                      \
    } else                                                                     \
        LOG_FMT(severity, format, ##__VA_ARGS__)

#define LOGD_FMT_IF(condition, format, ...)                                    \
    LOG_FMT_IF(glue::log::debug, condition, format, ##__VA_ARGS__)
#define LOGI_FMT_IF(condition, format, ...)                                    \
    LOG_FMT_IF(glue::log::info, condition, format, ##__VA_ARGS__)
#define LOGW_FMT_IF(condition, format, ...)                                    \
    LOG_FMT_IF(glue::log::warn, condition, format, ##__VA_ARGS__)
#define LOGE_FMT_IF(condition, format, ...)                                    \
    LOG_FMT_IF(glue::log::error, condition, format, ##__VA_ARGS__)
#define LOGF_FMT_IF(condition, format, ...)                                    \
    LOG_FMT_IF(glue::log::fatal, condition, format, ##__VA_ARGS__)

#ifdef LOG_DISABLE_LOGGING
    #define CHECK(x)                                                           \
        if (!(x)) {                                                            \
            throw std::runtime_error("Check failed: " #x);                     \
        }
    #define _CHECK_BINARY(x, cmp, y) CHECK(x cmp y)
#else
    #define CHECK(x) LOGF_IF(!(x)) << "Check failed: \"" #x << ": "
    #define _CHECK_BINARY(x, cmp, y)                                           \
        CHECK((x cmp y)) << (x) << "!" #cmp << (y) << "\" " // NOLINT(*)
#endif

#define CHECK_EQ(x, y) _CHECK_BINARY(x, ==, y)
#define CHECK_NE(x, y) _CHECK_BINARY(x, !=, y)
#define CHECK_LT(x, y) _CHECK_BINARY(x, <, y)
#define CHECK_LE(x, y) _CHECK_BINARY(x, <=, y)
#define CHECK_GT(x, y) _CHECK_BINARY(x, >, y)
#define CHECK_GE(x, y) _CHECK_BINARY(x, >=, y)
