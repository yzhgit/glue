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
namespace logging {

/** Base Appender class to be inherited by other Appender classes */
class LogSink {
  public:
    /** Default Constructor */
    LogSink() noexcept : mtx_() {}

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
    virtual void log(const std::string &msg) = 0;

  private:
    std::mutex mtx_;
};

/** Console Appender */
class ConsoleSink final : public LogSink {
  public:
    ConsoleSink();
    ~ConsoleSink();

    void log(const std::string &msg) override;

  private:
    const bool isatty_;
    std::ostream &outputStream_;
#ifdef _WIN32
    HANDLE outputHandle_;
#endif
};

/** File Appender */
class FileSink final : public LogSink {
  public:
    FileSink(const std::string &filename);
    ~FileSink();

    void log(const std::string &msg) override;

  private:
    std::ofstream outputStream_;
};

} // namespace logging
} // namespace glue
