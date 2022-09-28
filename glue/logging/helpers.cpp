//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/logging/helpers.h"

#include <ctime>
#include <map>
#include <sstream>
#include <string>

#if _WIN32
    #include <Windows.h>
#endif

namespace glue {
namespace log {

const char *severityToString(LogLevel severity) noexcept {
    switch (severity) {
    case LogLevel::FATAL:
        return "F";
    case LogLevel::ERROR:
        return "E";
    case LogLevel::WARN:
        return "W";
    case LogLevel::INFO:
        return "I";
    case LogLevel::DEBUG:
        return "D";
    default:
        return "-";
    }
}

// Return current thread id as size_t
// It exists because the std::this_thread::get_id() is much slower(especially
// under VS 2013)
size_t _thread_id() noexcept {
#ifdef _WIN32
    return static_cast<size_t>(::GetCurrentThreadId());
#elif defined(__linux__)
    #if defined(__ANDROID__) && defined(__ANDROID_API__) &&                    \
        (__ANDROID_API__ < 21)
        #define SYS_gettid __NR_gettid
    #endif
    return static_cast<size_t>(::syscall(SYS_gettid));
#elif defined(_AIX)
    struct __pthrdsinfo buf;
    int reg_size = 0;
    pthread_t pt = pthread_self();
    int retval = pthread_getthrds_np(&pt, PTHRDSINFO_QUERY_TID, &buf,
                                     sizeof(buf), NULL, &reg_size);
    int tid = (!retval) ? buf.__pi_tid : 0;
    return static_cast<size_t>(tid);
#elif defined(__DragonFly__) || defined(__FreeBSD__)
    return static_cast<size_t>(::pthread_getthreadid_np());
#elif defined(__NetBSD__)
    return static_cast<size_t>(::_lwp_self());
#elif defined(__OpenBSD__)
    return static_cast<size_t>(::getthrid());
#elif defined(__sun)
    return static_cast<size_t>(::thr_self());
#elif __APPLE__
    uint64_t tid;
    pthread_threadid_np(nullptr, &tid);
    return static_cast<size_t>(tid);
#else // Default to standard C++11 (other Unix)
    return static_cast<size_t>(
        std::hash<std::thread::id>()(std::this_thread::get_id()));
#endif
}

// Return current thread id as size_t (from thread local storage)
size_t thread_id() noexcept {
#if defined(GL_NO_TLS)
    return _thread_id();
#else // cache thread id in tls
    static thread_local const size_t tid = _thread_id();
    return tid;
#endif
}

#if _WIN32
int gettimeofday(struct timeval *tp, void *tzp) {
    LARGE_INTEGER now, freq;
    QueryPerformanceCounter(&now);
    QueryPerformanceFrequency(&freq);
    tp->tv_sec = now.QuadPart / freq.QuadPart;
    tp->tv_usec = (now.QuadPart % freq.QuadPart) * 1000000 / freq.QuadPart;

    return (0);
}
#endif

LogSource::LogSource(LogLevel severity, const char *filename, uint32_t linenum)
    : m_severity(severity), m_filename(filename), m_linenum(linenum) {}

std::string LogSource::toString() const {
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
