//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/logging/log.h"
#include <deque>

using namespace glue;

int main(int argc, char *argv[]) {
    int a = 10;
    float b = 3.1;

    static log::ConsoleSink<log::DefaultFormatterUtc> consoleSink;
    log::init<log::DefaultFormatter>(log::debug, "1.log").addSink(&consoleSink);
    LOGD << "hello world";

    LOGI << "You shouldn't see this message";

    LOGE << "a = " << a << " b = " << b << std::string(" This is an apple!!!");

    LOGW_FMT("a = %d, b= %.3f", a, b);

    CHECK_LT(a, b) << " exit!!!";

    // Conditional logging.
    int var = 1;
    LOGD_IF(var != 0) << "You shouldn't see this message";
    LOGD_IF(var == 0) << "This is a conditional log message";

    LOGI_FMT_IF(var != 0, "You shouldn't see this message var != 0");
    LOGE_FMT_IF(var == 0, "This is a conditional log message because %d == 0", var);
}
