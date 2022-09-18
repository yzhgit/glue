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

#include "logging/types.h"

#include <string>

namespace glue {
namespace logging {

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
const char *severityToString(LogSeverity severity);

// Return current thread id as size_t (from thread local storage)
size_t thread_id() noexcept;

} // namespace logging
} // namespace glue
