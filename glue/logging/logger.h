//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/logging/helpers.h"
#include "glue/logging/sink.h"
#include "glue/logging/types.h"

#include <memory>
#include <string>
#include <vector>

namespace glue {
namespace log {

/** Logger class */
class Logger {
  public:
    /** Default Constructor
     *
     * @param[in] name      Name of the logger
     */
    Logger(const char *name);

    /** Prevent instances of this class from being copied (As this class
     * contains pointers) */
    Logger(const Logger &) = delete;
    /** Prevent instances of this class from being copied (As this class
     * contains pointers) */
    Logger &operator=(const Logger &) = delete;

    /** Allow instances of this class to be moved */
    Logger(Logger &&) = default;
    /** Allow instances of this class to be moved */
    Logger &operator=(Logger &&) = default;

    /** Add sink
     *
     * @param[in] sink   Sink to push the messages
     */
    void addSink(std::shared_ptr<LogSink> sink);

    /** Logs a message
     *
     * @param[in] severity Log level of the message
     * @param[in] msg       Message to log
     */
    void log(LogLevel severity, const char *file, int line, const char *msg);

    /** Logs a formatted message
     *
     * @param[in] severity Log level of the message
     * @param[in] fmt       Message format
     * @param[in] args      Message arguments
     */
    template <typename... Args>
    void log(LogLevel severity, const char *file, int line, const char *fmt,
             Args... args);

    /** Sets log level of the logger
     *
     * @warning Not thread-safe
     *
     * @param[in] severity Log level to set
     */
    void setSeverity(LogLevel severity);
    /** Returns logger's log level
     *
     * @return Logger's log level
     */
    LogLevel severity() const;

    /** Returns logger's name
     *
     * @return Logger's name
     */
    const char *name() const;

  private:
    /** Checks if a message should be logged depending
     *  on the message log level and the loggers one
     *
     * @param[in] severity Log level
     *
     * @return True if message should be logged else false
     */
    bool isLoggable(LogLevel severity) { return severity >= m_severity; }

    /** Prints the message to all the printers
     *
     * @param[in] msg Message to print
     */
    void writeToSinks(const char *name, const LogSource &source,
                      const std::string &msg);

  private:
    const char *m_name;
    LogLevel m_severity;
    std::vector<std::shared_ptr<LogSink>> m_sinks;
};

template <typename... Args>
inline void Logger::log(LogLevel severity, const char *file, int line,
                        const char *fmt, Args... args) {
    if (isLoggable(severity)) {
        writeToSinks(m_name, LogSource(severity, file, line),
                     formatString(fmt, args...));
    }
}

Logger* GetCurrentLogger();
void InitLogger(bool console, const char *file);

} // namespace log
} // namespace glue
