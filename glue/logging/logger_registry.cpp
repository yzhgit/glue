//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/logging/logger_registry.h"

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
