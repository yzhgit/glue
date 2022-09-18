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

#include "logging/helpers.h"

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
