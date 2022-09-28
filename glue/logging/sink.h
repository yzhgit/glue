//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/logging/record.h"

#include <fstream>
#include <iostream>
#include <mutex>

namespace glue {
namespace log {

using LogSinkPtr = std::shared_ptr<LogSink>;

/** Base Appender class to be inherited by other Appender classes */
class LogSink {
  public:
    /** Defaults Destructor */
    virtual ~LogSink() = default;

    /** Print message
     *
     * @param[in] msg Message to print
     */
    virtual void write(const Record &record) = 0;
};

/** Console Appender */
class ConsoleSink final : public LogSink {
  public:
    ConsoleSink();
    ~ConsoleSink();

    void write(const Record &record) override;

  private:
    const bool m_isatty;
    std::mutex m_mutex;
    std::ostream &m_outputStream;
};

/** File Appender */
class FileSink final : public LogSink {
  public:
    FileSink(const std::string &filename);
    ~FileSink();

    void write(const Record &record) override;

  private:
    std::ofstream m_outputStream;
};

} // namespace log
} // namespace glue
