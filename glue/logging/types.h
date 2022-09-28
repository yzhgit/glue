//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include <string>

namespace glue {
namespace log {

/** Logging level enumeration */
enum class LogLevel {
    DEBUG,
    INFO,  /**< Information log level */
    WARN,  /**< Warning log level */
    ERROR, /**< Error log level */
    FATAL,
    OFF /**< No logging */
};

/** Log message */
class LogSource {
  public:
    /** Construct a log message
     *
     * @param[in] msg       Message to log.
     * @param[in] severity Logging level. Default: OFF
     */
    LogSource(LogLevel severity, const char *filename, uint32_t linenum);

    LogLevel severity() const { return m_severity; }

    std::string toString() const;

  private:
    LogLevel m_severity;
    const char *m_filename;
    uint32_t m_linenum;
};

} // namespace log
} // namespace glue
