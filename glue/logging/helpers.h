//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include <string>

#ifdef _WIN32
    #include <io.h>
    #include <share.h>
    #include <sys/timeb.h>
    #include <time.h>
#else
    #include <sys/time.h>
    #include <unistd.h>
    #if defined(__linux__) || defined(__FreeBSD__)
        #include <sys/syscall.h>
    #elif defined(__rtems__)
        #include <rtems.h>
    #endif
    #if defined(_POSIX_THREADS)
        #include <pthread.h>
    #endif
#endif

#ifdef _WIN32
    #define _LOG_NSTR(x) L##x
    #define LOG_NSTR(x) _LOG_NSTR(x)
#else
    #define LOG_NSTR(x) x
#endif

#ifdef _WIN32
    #define LOG_CDECL __cdecl
#else
    #define LOG_CDECL
#endif

namespace glue {
namespace log {

/** Create a string given a format
 *
 * @param[in] fmt  String format
 * @param[in] args Arguments
 *
 * @return The formatted string
 */
template <typename... Args>
inline std::string formatString(const char *fmt, Args... args) {
    size_t length = std::snprintf(nullptr, 0, fmt, args...);
    std::string str;
    str.reserve(length + 1);
    std::snprintf(const_cast<char *>(str.data()), length + 1, fmt, args...);
    return str;
}

namespace util {
#ifdef _WIN32
typedef std::wstring nstring;
typedef std::wostringstream nostringstream;
typedef std::wistringstream nistringstream;
typedef wchar_t nchar;
#else
typedef std::string nstring;
typedef std::ostringstream nostringstream;
typedef std::istringstream nistringstream;
typedef char nchar;
#endif

inline void localtime_s(struct tm *t, const time_t *time) {
#if defined(_WIN32) && defined(__BORLANDC__)
    ::localtime_s(time, t);
#elif defined(_WIN32) && defined(__MINGW32__) &&                               \
    !defined(__MINGW64_VERSION_MAJOR)
    *t = *::localtime(time);
#elif defined(_WIN32)
    ::localtime_s(t, time);
#else
    ::localtime_r(time, t);
#endif
}

inline void gmtime_s(struct tm *t, const time_t *time) {
#if defined(_WIN32) && defined(__BORLANDC__)
    ::gmtime_s(time, t);
#elif defined(_WIN32) && defined(__MINGW32__) &&                               \
    !defined(__MINGW64_VERSION_MAJOR)
    *t = *::gmtime(time);
#elif defined(_WIN32)
    ::gmtime_s(t, time);
#else
    ::gmtime_r(time, t);
#endif
}

#ifdef _WIN32
typedef timeb Time;

inline void ftime(Time *t) { ::ftime(t); }
#else
struct Time {
    time_t time;
    unsigned short millitm;
};

inline void ftime(Time *t) {
    timeval tv;
    ::gettimeofday(&tv, NULL);

    t->time = tv.tv_sec;
    t->millitm = static_cast<unsigned short>(tv.tv_usec / 1000);
}
#endif

// Return current thread id as size_t
// It exists because the std::this_thread::get_id() is much slower(especially
// under VS 2013)
inline size_t _thread_id() noexcept {
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

inline size_t gettid() {
#if defined(GL_NO_TLS)
    return _thread_id();
#else // cache thread id in tls
    static thread_local const size_t tid = _thread_id();
    return tid;
#endif
}

#ifdef _WIN32
inline int vasprintf(char **strp, const char *format, va_list ap) {
    int charCount = _vscprintf(format, ap);
    if (charCount < 0) {
        return -1;
    }

    size_t bufferCharCount = static_cast<size_t>(charCount) + 1;

    char *str = static_cast<char *>(malloc(bufferCharCount));
    if (!str) {
        return -1;
    }

    #if defined(__MINGW32__) && !defined(__MINGW64_VERSION_MAJOR)
    int retval = _vsnprintf(str, bufferCharCount, format, ap);
    #else
    int retval = _vsnprintf_s(str, bufferCharCount, charCount, format, ap);
    #endif
    if (retval < 0) {
        free(str);
        return -1;
    }

    *strp = str;
    return retval;
}
#endif
} // namespace util

} // namespace log
} // namespace glue
