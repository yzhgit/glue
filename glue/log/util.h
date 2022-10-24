//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include <assert.h>
#include <stdio.h>
#include <time.h>

#if defined(_WIN32)
    #include <Windows.h>
    #include <sys/timeb.h>
#else
    #include <sys/time.h>
    #include <unistd.h>
    #if defined(__linux__)
        #include <sys/syscall.h>
    #endif
    #if defined(_POSIX_THREADS)
        #include <pthread.h>
    #endif
#endif

namespace glue
{
namespace log
{

    namespace util
    {

        inline void localtime_s(struct tm* t, const time_t* time)
        {
#if defined(_WIN32)
            ::localtime_s(t, time);
#else
            ::localtime_r(time, t);
#endif
        }

        inline void gmtime_s(struct tm* t, const time_t* time)
        {
#if defined(_WIN32)
            ::gmtime_s(t, time);
#else
            ::gmtime_r(time, t);
#endif
        }

        // Return current thread id as size_t
        // It exists because the std::this_thread::get_id() is much slower(especially
        // under VS 2013)
        inline size_t _thread_id() noexcept
        {
#if defined(_WIN32)
            return static_cast<size_t>(::GetCurrentThreadId());
#elif defined(__linux__)
    #if defined(__ANDROID__) && defined(__ANDROID_API__) && (__ANDROID_API__ < 21)
        #define SYS_gettid __NR_gettid
    #endif
            return static_cast<size_t>(::syscall(SYS_gettid));
#else // Default to standard C++11 (other Unix)
            return static_cast<size_t>(std::hash<std::thread::id>()(std::this_thread::get_id()));
#endif
        }

        inline size_t gettid()
        {
#if defined(GL_NO_TLS)
            return _thread_id();
#else // cache thread id in tls
            static thread_local const size_t tid = _thread_id();
            return tid;
#endif
        }

#if defined(_WIN32)
        inline int vasprintf(char** strp, const char* format, va_list ap)
        {
            int charCount = _vscprintf(format, ap);
            if (charCount < 0) { return -1; }

            size_t bufferCharCount = static_cast<size_t>(charCount) + 1;
            char* str = static_cast<char*>(malloc(bufferCharCount));
            if (!str) { return -1; }

            int retval = _vsnprintf_s(str, bufferCharCount, charCount, format, ap);
            if (retval < 0)
            {
                free(str);
                return -1;
            }

            *strp = str;
            return retval;
        }
#endif

        class NonCopyable
        {
        protected:
            NonCopyable() {}

        private:
            NonCopyable(const NonCopyable&);
            NonCopyable& operator=(const NonCopyable&);
        };

        template <class T>
        class Singleton : NonCopyable
        {
        public:
#if defined(__clang__) || __GNUC__ >= 8
            // This constructor is called before the `T` object is fully constructed,
            // and pointers are not dereferenced anyway, so UBSan shouldn't check vptrs.
            __attribute__((no_sanitize("vptr")))
#endif
            Singleton()
            {
                assert(!m_instance);
                m_instance = static_cast<T*>(this);
            }

            ~Singleton()
            {
                assert(m_instance);
                m_instance = 0;
            }

            static T* getInstance() { return m_instance; }

        private:
            static T* m_instance;
        };

        template <class T>
        T* Singleton<T>::m_instance = NULL;

    } // namespace util

} // namespace log
} // namespace glue
