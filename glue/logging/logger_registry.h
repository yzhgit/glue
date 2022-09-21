//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/logging/log_sink.h"
#include "glue/logging/logger.h"
#include "glue/logging/types.h"

#include <memory>
#include <mutex>
#include <set>
#include <unordered_map>

namespace glue {
namespace logging {

/** Registry class holding all the instantiated loggers */
class LoggerRegistry final {
  public:
    /** Gets registry instance
     *
     * @return Logger registry instance
     */
    static LoggerRegistry &get();

    /** Add a logger
     *
     * @param[in] logger  logger to attach to the system loggers.
     */
    void addLogger(std::shared_ptr<Logger> &logger);

    /** Returns a logger instance
     *
     * @param[in] name Logger to return
     *
     * @return Logger
     */
    std::shared_ptr<Logger> logger(const std::string &name);

  private:
    /** Default constructor */
    LoggerRegistry();

  private:
    std::mutex mtx_;
    std::unordered_map<std::string, std::shared_ptr<Logger>> loggers_;
};

} // namespace logging
} // namespace glue
