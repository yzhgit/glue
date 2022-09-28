//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/logging/helpers.h"

#include <fstream>
#include <iostream>
#include <mutex>

#ifdef _WIN32
    #include <Windows.h>
    #include <io.h>
#endif

namespace glue {
namespace log {

/** Base Appender class to be inherited by other Appender classes */
class LogSink {
  public:
    /** Default Constructor */
    LogSink() noexcept : m_mutex() {}

    /** Prevent instances of this class from being copied */
    LogSink(const LogSink &) = delete;
    /** Prevent instances of this class from being copied */
    LogSink &operator=(const LogSink &) = delete;
    /** Prevent instances of this class from being moved */
    LogSink(LogSink &&) = delete;
    /** Prevent instances of this class from being moved */
    LogSink &operator=(LogSink &&) = delete;
    /** Defaults Destructor */
    virtual ~LogSink() = default;

    /** Print message
     *
     * @param[in] msg Message to print
     */
    virtual void write(const char *name, LogLevel severity,
                       const std::string &prefix, const std::string &msg) = 0;

  protected:
    std::mutex m_mutex;
};

/** Console Appender */
class ConsoleSink final : public LogSink {
  public:
    ConsoleSink();
    ~ConsoleSink();

    void write(const char *name, LogLevel severity, const std::string &prefix,
               const std::string &msg) override;

  private:
    const bool m_isatty;
    std::ostream &m_outputStream;
};

/** File Appender */
class FileSink final : public LogSink {
  public:
    FileSink(const std::string &filename);
    ~FileSink();

    void write(const char *name, LogLevel severity, const std::string &prefix,
               const std::string &msg) override;

  private:
    std::ofstream m_outputStream;
};

} // namespace log
} // namespace glue
