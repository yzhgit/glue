//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/logging/record.h"
#include "glue/logging/sink.h"
#include "glue/logging/util.h"

#include <memory>
#include <vector>

namespace glue {
namespace log {

class Logger : public util::Singleton<Logger> {
  public:
    /** Default Constructor
     *
     * @param[in] maxSeverity      Name of the logger
     */
    Logger(Severity maxSeverity = off) : m_maxSeverity(maxSeverity) {}

    /** Add sink
     *
     * @param[in] sink   Sink to push the messages
     */
    Logger &addSink(LogSink *sink) {
        m_sinks.push_back(sink);
        return *this;
    }

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

    void log(Severity severity, size_t line, const char *file, const char *format, ...) {
        using namespace util;

        char *str = NULL;
        va_list ap;

        va_start(ap, format);
        int len = vasprintf(&str, format, ap);
        static_cast<void>(len);
        va_end(ap);

        Record record(severity, line, file);
        record << str;
        *this += record;

        free(str);
    }

    void operator+=(const Record &record) {
        for (std::vector<LogSink *>::iterator it = m_sinks.begin(); it != m_sinks.end(); ++it) {
            (*it)->write(record);
        }
    }

  private:
    Severity m_maxSeverity;
#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable : 4251) // needs to have dll-interface to be used by
                                    // clients of class
#endif
    std::vector<LogSink *> m_sinks;
#ifdef _MSC_VER
    #pragma warning(pop)
#endif
};

inline Logger *get() { return Logger::getInstance(); }

} // namespace log
} // namespace glue
