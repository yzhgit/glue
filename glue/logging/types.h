//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include <string>
#include <time.h>

namespace glue {
namespace logging {

/** Logging level enumeration */
enum class LogSeverity {
    VERBOSE, /**< All logging messages */
    DEBUG,
    INFO,  /**< Information log level */
    WARN,  /**< Warning log level */
    ERROR, /**< Error log level */
    FATAL,
    OFF /**< No logging */
};

/** Log message */
struct LogMessage {
    /** Construct a log message
     *
     * @param[in] msg       Message to log.
     * @param[in] severity Logging level. Default: OFF
     */
    LogMessage(const char *name, LogSeverity severity, const char *file,
               int line, const char *func, std::string msg);

    const char *name_;
    /** Logging level */
    LogSeverity severity_;
    const char *file_;
    int line_;
    const char *func_;
    /** Log message */
    std::string raw_;
    time_t time_;
    size_t thread_id_;
};

} // namespace logging
} // namespace glue
