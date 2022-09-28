//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

namespace glue {
namespace log {

/** Logging level enumeration */
enum class Severity {
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
    case Severity::fatal:
        return "F";
    case Severity::error:
        return "E";
    case Severity::warn:
        return "W";
    case Severity::info:
        return "I";
    case Severity::debug:
        return "D";
    default:
        return "-";
    }
}

} // namespace log
} // namespace glue
