//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/logging/android_sink.h"
#include "glue/logging/console_sink.h"
#include "glue/logging/file_sink.h"
#include "glue/logging/formatter.h"
#include "glue/logging/logger.h"

namespace glue {
namespace log {

inline Logger &init(Severity maxSeverity = off, LogSink *sink = NULL) {
    static Logger logger(maxSeverity);
    return sink ? logger.addSink(sink) : logger;
}

template <class Formatter>
inline Logger &init(Severity maxSeverity) {
    static ConsoleSink<Formatter> sink;
    return init(maxSeverity, &sink);
}

template <class Formatter>
inline Logger &init(Severity maxSeverity, const char *fileName) {
    static FileSink<Formatter> sink(fileName);
    return init(maxSeverity, &sink);
}

} // namespace log
} // namespace glue
