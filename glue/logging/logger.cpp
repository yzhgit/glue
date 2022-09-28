//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/logging/logger.h"

#include <iomanip>
#include <sstream>

namespace glue {
namespace log {

Logger::Logger(const char *name) : m_name(name) {}

void Logger::addSink(std::shared_ptr<LogSink> sink) { m_sinks.push_back(sink); }

void Logger::log(LogLevel severity, const char *file, int line,
                 const char *msg) {
    if (isLoggable(severity)) {
        writeToSinks(m_name, LogSource(severity, file, line), msg);
    }
}

void Logger::setSeverity(LogLevel severity) { m_severity = severity; }

LogLevel Logger::severity() const { return m_severity; }

const char *Logger::name() const { return m_name; }

void Logger::writeToSinks(const char *name, const LogSource &source,
                          const std::string &msg) {
    std::string prefix = source.toString();
    for (auto &sink : m_sinks) {
        sink->write(name, source.severity(), prefix, msg);
    }
}

void InitLogger(bool console, const char *file) {
    auto color_sink = std::make_shared<ConsoleSink>();
    auto file_sink = std::make_shared<FileSink>("1.log");
}

} // namespace log
} // namespace glue
