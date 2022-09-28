//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/logging/sink.h"
#include "glue/logging/types.h"

#if defined(ANDROID)
    #include <android/log.h>
#endif

namespace glue {
namespace log {

ConsoleSink::ConsoleSink()
    : m_isatty(!!_isatty(_fileno(stdout))), m_outputStream(std::cout) {}

ConsoleSink::~ConsoleSink() {}

void ConsoleSink::write(const char *name, LogLevel severity,
                        const std::string &prefix, const std::string &msg) {
#if defined(ANDROID)
    switch (severity) {
    case LOG_LEVEL_DEBUG:
        __android_log_print(ANDROID_LOG_DEBUG, name, "%s", msg.c_str());
        break;
    case LOG_LEVEL_INFO:
        __android_log_print(ANDROID_LOG_INFO, name, "%s", msg.c_str());
        break;
    case LOG_LEVEL_WARN:
        __android_log_print(ANDROID_LOG_WARN, name, "%s", msg.c_str());
        break;
    case LOG_LEVEL_ERROR:
        __android_log_print(ANDROID_LOG_ERROR, name, "%s", msg.c_str());
        break;
    case LOG_LEVEL_FATAL:
        __android_log_print(ANDROID_LOG_FATAL, name, "%s", msg.c_str());
        break;
    default:
        break;
    }
#else
    m_outputStream << prefix << msg << '\n';
    m_outputStream.flush();
#endif
}

FileSink::FileSink(const std::string &filename) {
    m_outputStream.open(filename, std::fstream::out | std::fstream::trunc);
}

FileSink::~FileSink() { m_outputStream.close(); }

void FileSink::write(const char *name, LogLevel severity,
                     const std::string &prefix, const std::string &msg) {
    m_outputStream << prefix << msg << '\n';
    m_outputStream.flush();
}

} // namespace log
} // namespace glue
