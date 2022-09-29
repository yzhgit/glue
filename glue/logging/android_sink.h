//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/logging/record.h"
#include "glue/logging/sink.h"

#if defined(ANDROID)
    #include <android/log.h>
#endif

namespace glue {
namespace log {

#if defined(ANDROID)
template <class Formatter>
class AndroidSink : public LogSink {
  public:
    void write(const Record &record) override {
        std::string str = Formatter::format(record);
        switch (record.getSeverity()) {
        case debug:
            __android_log_print(ANDROID_LOG_DEBUG, name, "%s", str.c_str());
            break;
        case info:
            __android_log_print(ANDROID_LOG_INFO, name, "%s", str.c_str());
            break;
        case warn:
            __android_log_print(ANDROID_LOG_WARN, name, "%s", str.c_str());
            break;
        case error:
            __android_log_print(ANDROID_LOG_ERROR, name, "%s", str.c_str());
            break;
        case fatal:
            __android_log_print(ANDROID_LOG_FATAL, name, "%s", str.c_str());
            break;
        default:
            break;
        }
    }
};
#endif

} // namespace log
} // namespace glue
