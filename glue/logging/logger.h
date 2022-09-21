//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/logging/helpers.h"
#include "glue/logging/log_sink.h"
#include "glue/logging/types.h"

#include <memory>
#include <string>
#include <vector>

namespace glue {
namespace logging {

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
    void log(LogSeverity severity, const char *file, int line, const char *func,
             const char *msg);

    /** Logs a formatted message
     *
     * @param[in] severity Log level of the message
     * @param[in] fmt       Message format
     * @param[in] args      Message arguments
     */
    template <typename... Args>
    void log(LogSeverity severity, const char *file, int line, const char *func,
             const char *fmt, Args... args);

    /** Sets log level of the logger
     *
     * @warning Not thread-safe
     *
     * @param[in] severity Log level to set
     */
    void setSeverity(LogSeverity severity);
    /** Returns logger's log level
     *
     * @return Logger's log level
     */
    LogSeverity severity() const;

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
    bool isLoggable(LogSeverity severity) { return severity >= severity_; }

    /** Prints the message to all the printers
     *
     * @param[in] msg Message to print
     */
    void writeToSinks(const LogMessage &msg);

    std::string formatMessage(const LogMessage &msg);

  private:
    const char *name_;
    LogSeverity severity_;
    std::vector<std::shared_ptr<LogSink>> sinks_;
};

template <typename... Args>
inline void Logger::log(LogSeverity severity, const char *file, int line,
                        const char *func, const char *fmt, Args... args) {
    if (isLoggable(severity)) {
        writeToSinks(LogMessage(name_, severity, file, line, func,
                                formatString(fmt, args...)));
    }
}

} // namespace logging
} // namespace glue
