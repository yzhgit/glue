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

#include "logging/logger_registry.h"

namespace glue {
namespace logging {

LoggerRegistry::LoggerRegistry() : mtx_(), loggers_() {}

LoggerRegistry &LoggerRegistry::get() {
    static LoggerRegistry instance_;
    return instance_;
}

void LoggerRegistry::addLogger(std::shared_ptr<Logger> &logger) {
    std::lock_guard<std::mutex> lock(mtx_);
    std::string name = logger->name();
    if ((loggers_.find(name) == loggers_.end())) {
        loggers_[name] = logger;
    }
}

std::shared_ptr<Logger> LoggerRegistry::logger(const std::string &name) {
    std::lock_guard<std::mutex> lock(mtx_);
    return (loggers_.find(name) != loggers_.end()) ? loggers_[name] : nullptr;
}

} // namespace logging
} // namespace glue
