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

#pragma once

#include "logging/log_sink.h"
#include "logging/logger.h"
#include "logging/types.h"

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
