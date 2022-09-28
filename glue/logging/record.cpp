//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/logging/record.h"

namespace glue {
namespace log {

std::string Record::toString() const {
    std::stringstream ss;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    int msec = tv.tv_usec / 1000;

    time_t now;
    time(&now);

    char time_buf[30] = {0};
    struct tm *ptm = localtime(&now);
    snprintf(time_buf, 30, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
             ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour,
             ptm->tm_min, ptm->tm_sec, msec);

    ss << '[' << severityToString(m_severity) << ' ' << time_buf << ' '
       << thread_id() << ' ' << m_filename << ':' << m_linenum << ']';

    return ss.str();
}

} // namespace log
} // namespace glue
