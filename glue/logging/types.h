/*
 * Copyright 2015 zhangyao<mosee.gd@163.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
