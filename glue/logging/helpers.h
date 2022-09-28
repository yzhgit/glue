//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/logging/types.h"

#include <string>

namespace glue {
namespace log {

/** Create a string given a format
 *
 * @param[in] fmt  String format
 * @param[in] args Arguments
 *
 * @return The formatted string
 */
template <typename... Args>
inline std::string formatString(const char *fmt, Args... args) {
    size_t length = std::snprintf(nullptr, 0, fmt, args...);
    std::string str;
    str.reserve(length + 1);
    std::snprintf(const_cast<char *>(str.data()), length + 1, fmt, args...);
    return str;
}

/** Translates a given log level to a string.
 *
 * @param[in] severity @ref LogSeverity to be translated to string.
 *
 * @return The string describing the logging level.
 */
const char *severityToString(LogLevel severity) noexcept;

// Return current thread id as size_t (from thread local storage)
size_t thread_id() noexcept;

} // namespace log
} // namespace glue
