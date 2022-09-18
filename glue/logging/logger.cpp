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

#include "logging/logger.h"

#include <iomanip>
#include <sstream>

namespace glue {
namespace logging {

Logger::Logger(const char *name) : name_(name) {}

void Logger::addSink(std::shared_ptr<LogSink> sink) { sinks_.push_back(sink); }

void Logger::log(LogSeverity severity, const char *file, int line,
                 const char *func, const char *msg) {
    if (isLoggable(severity)) {
        writeToSinks(LogMessage(name_, severity, file, line, func, msg));
    }
}

void Logger::setSeverity(LogSeverity severity) { severity_ = severity; }

LogSeverity Logger::severity() const { return severity_; }

const char *Logger::name() const { return name_; }

std::string Logger::formatMessage(const LogMessage &msg) {
    std::stringstream ss;

    char _time[25] = {0};
    struct tm *local_tm = localtime(&msg.time_);
    strftime(_time, 25, "%Y-%m-%d %H:%M:%S", local_tm);

    ss << _time << " " << msg.thread_id_ << " " << std::setw(5)
       << severityToString(msg.severity_) << " " << msg.file_ << ":"
       << msg.line_ << " " << msg.func_ << " " << msg.raw_.c_str() << "\n";

    return ss.str();
}

void Logger::writeToSinks(const LogMessage &msg) {
    std::string formatStr = formatMessage(msg);
    for (auto &s : sinks_) {
        s->log(formatStr);
    }
}

} // namespace logging
} // namespace glue
