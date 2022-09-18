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

#define LOG "MYLOG"

using namespace glue::logging;

constexpr const char *Basename(const char *fname, int offset) {
    return offset == 0 || fname[offset - 1] == '/' || fname[offset - 1] == '\\'
               ? fname + offset
               : Basename(fname, offset - 1);
}

void test_log(std::shared_ptr<Logger> logger) {
    logger->log(LogSeverity::WARN, Basename(__FILE__, sizeof(__FILE__) - 1),
                __LINE__, __FUNCTION__, "This is warn message");
}

int main(int argc, char *argv[]) {
    int a = 10;
    float b = 3.1;

    auto color_sink = std::make_shared<ConsoleSink>();
    auto file_sink = std::make_shared<FileSink>("1.log");
    auto logger = std::make_shared<Logger>(LOG);
    logger->addSink(color_sink);
    logger->addSink(file_sink);

    logger->log(LogSeverity::DEBUG, Basename(__FILE__, sizeof(__FILE__) - 1),
                __LINE__, __FUNCTION__, "a = %d, b = %f", a, b);

    logger->log(LogSeverity::INFO, Basename(__FILE__, sizeof(__FILE__) - 1),
                __LINE__, __FUNCTION__, "This is info message");

    logger->log(LogSeverity::WARN, Basename(__FILE__, sizeof(__FILE__) - 1),
                __LINE__, __FUNCTION__, "This is warn message");

    test_log(logger);
}
