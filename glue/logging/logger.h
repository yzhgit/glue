//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/logging/helpers.h"
#include "glue/logging/sink.h"

#include <memory>
#include <string>
#include <vector>

namespace glue {
namespace log {

class Logger {
  public:
    /** Default Constructor
     *
     * @param[in] maxSeverity      Name of the logger
     */
    Logger(Severity maxSeverity = Severity::off) : m_maxSeverity(maxSeverity) {}

    /** Add sink
     *
     * @param[in] sink   Sink to push the messages
     */
    Logger &addSink(LogSinkPtr sink) {
        m_sinks.push_back(sink);
        return *this;
    }

    // /** Logs a message
    //  *
    //  * @param[in] severity Log level of the message
    //  * @param[in] msg       Message to log
    //  */
    // void log(Severity severity, const char *file, int line, const char *msg);

    // /** Logs a formatted message
    //  *
    //  * @param[in] severity Log level of the message
    //  * @param[in] fmt       Message format
    //  * @param[in] args      Message arguments
    //  */
    // template <typename... Args>
    // void log(Severity severity, const char *file, int line, const char *fmt,
    //          Args... args);

    /** Sets log level of the logger
     *
     * @warning Not thread-safe
     *
     * @param[in] severity Log level to set
     */
    void setMaxSeverity(Severity severity) { m_maxSeverity = severity; }
    /** Returns logger's log level
     *
     * @return Logger's log level
     */
    Severity getMaxSeverity() const { return m_maxSeverity; }

    /** Checks if a message should be logged depending
     *  on the message log level and the loggers one
     *
     * @param[in] severity Log level
     *
     * @return True if message should be logged else false
     */
    bool checkSeverity(Severity severity) { return severity >= m_maxSeverity; }

    virtual void write(const Record &record) {
        if (checkSeverity(record.getSeverity())) {
            *this += record;
        }
    }

    void operator+=(const Record &record) {
        for (std::vector<LogSinkPtr>::iterator it = m_sinks.begin();
             it != m_sinks.end(); ++it) {
            it->write(record);
        }
    }

  private:
    // /** Prints the message to all the printers
    //  *
    //  * @param[in] msg Message to print
    //  */
    // void writeToSinks(const char *name, const Record &source,
    //                   const std::string &msg);

  private:
    Severity m_maxSeverity;
#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable : 4251) // needs to have dll-interface to be used by
                                    // clients of class
#endif
    std::vector<LogSinkPtr> m_sinks;
#ifdef _MSC_VER
    #pragma warning(pop)
#endif
};

// template <typename... Args>
// inline void Logger::log(Severity severity, const char *file, int line,
//                         const char *fmt, Args... args) {
//     if (isLoggable(severity)) {
//         writeToSinks(m_name, Record(severity, file, line),
//                      formatString(fmt, args...));
//     }
// }

} // namespace log
} // namespace glue
