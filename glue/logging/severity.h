//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

namespace glue {
namespace log {

/** Logging level enumeration */
enum Severity {
    debug,
    info,  /**< Information log level */
    warn,  /**< Warning log level */
    error, /**< Error log level */
    fatal,
    off /**< No logging */
};

/** Translates a given log level to a string.
 *
 * @param[in] severity @ref LogSeverity to be translated to string.
 *
 * @return The string describing the logging level.
 */
inline const char *severityToString(Severity severity) {
    switch (severity) {
    case fatal:
        return "FATAL";
    case error:
        return "ERROR";
    case warn:
        return "WARN";
    case info:
        return "INFO";
    case debug:
        return "DEBUG";
    default:
        return "OFF";
    }
}

} // namespace log
} // namespace glue
