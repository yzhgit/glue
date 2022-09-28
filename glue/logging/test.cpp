//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/logging/logger.h"

#define LOG "MYLOG"

using namespace glue::logging;

constexpr const char *Basename(const char *fname, int offset) {
    return offset == 0 || fname[offset - 1] == '/' || fname[offset - 1] == '\\'
               ? fname + offset
               : Basename(fname, offset - 1);
}

void test_log(std::shared_ptr<Logger> logger) {
    logger->log(LogLevel::WARN, Basename(__FILE__, sizeof(__FILE__) - 1),
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

    logger->log(LogLevel::DEBUG, Basename(__FILE__, sizeof(__FILE__) - 1),
                __LINE__, __FUNCTION__, "a = %d, b = %f", a, b);

    logger->log(LogLevel::INFO, Basename(__FILE__, sizeof(__FILE__) - 1),
                __LINE__, __FUNCTION__, "This is info message");

    logger->log(LogLevel::WARN, Basename(__FILE__, sizeof(__FILE__) - 1),
                __LINE__, __FUNCTION__, "This is warn message");

    test_log(logger);
}
