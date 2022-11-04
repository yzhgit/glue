//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/base/Time.h"

GLUE_START_NAMESPACE

int64 Time::currentTimeMillis() noexcept
{
#if defined(GLUE_OS_WINDOWS) && !defined(GLUE_COMPILER_MINGW)
    struct _timeb t;
    _ftime_s(&t);
    return ((int64) t.time) * 1000 + t.millitm;
#else
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return ((int64) tv.tv_sec) * 1000 + tv.tv_usec / 1000;
#endif
}

double Time::highResolutionTicksToSeconds(const int64 ticks) noexcept
{
    return (double) ticks / (double) getHighResolutionTicksPerSecond();
}

int64 Time::secondsToHighResolutionTicks(const double seconds) noexcept
{
    return (int64) (seconds * (double) getHighResolutionTicksPerSecond());
}

#if defined(GLUE_OS_WINDOWS)

    #include <timeapi.h>
    #pragma comment(lib, "winmm.lib")

class HiResCounterHandler
{
public:
    HiResCounterHandler() : hiResTicksOffset(0)
    {
        // This macro allows you to override the default timer-period
        // used on Windows. By default this is set to 1, because that has
        // always been the value used in GLUE apps, and changing it could
        // affect the behaviour of existing code, but you may wish to make
        // it larger (or set it to 0 to use the system default) to make your
        // app less demanding on the CPU.
        // For more info, see win32 documentation about the timeBeginPeriod
        // function.
    #ifndef GLUE_WIN32_TIMER_PERIOD
        #define GLUE_WIN32_TIMER_PERIOD 1
    #endif

    #if GLUE_WIN32_TIMER_PERIOD > 0
        auto res = timeBeginPeriod(GLUE_WIN32_TIMER_PERIOD);
        GLUE_ASSERT(res == TIMERR_NOERROR);
    #endif

        LARGE_INTEGER f;
        QueryPerformanceFrequency(&f);
        hiResTicksPerSecond = f.QuadPart;
        hiResTicksScaleFactor = 1000.0 / (double) hiResTicksPerSecond;
    }

    inline int64 getHighResolutionTicks() noexcept
    {
        LARGE_INTEGER ticks;
        QueryPerformanceCounter(&ticks);
        return ticks.QuadPart + hiResTicksOffset;
    }

    inline double getMillisecondCounterHiRes() noexcept
    {
        return (double) getHighResolutionTicks() * hiResTicksScaleFactor;
    }

    int64 hiResTicksPerSecond, hiResTicksOffset;
    double hiResTicksScaleFactor;
};

static HiResCounterHandler hiResCounterHandler;

int64 Time::getHighResolutionTicksPerSecond() noexcept
{
    return hiResCounterHandler.hiResTicksPerSecond;
}
int64 Time::getHighResolutionTicks() noexcept
{
    return hiResCounterHandler.getHighResolutionTicks();
}
double Time::getMillisecondCounterHiRes() noexcept
{
    return hiResCounterHandler.getMillisecondCounterHiRes();
}

#elif defined(GLUE_OS_ANDROID)

int64 Time::getHighResolutionTicks() noexcept
{
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);

    return (t.tv_sec * (int64) 1000000) + (t.tv_nsec / 1000);
}

int64 Time::getHighResolutionTicksPerSecond() noexcept
{
    return 1000000; // (microseconds)
}

double Time::getMillisecondCounterHiRes() noexcept
{
    return (double) getHighResolutionTicks() * 0.001;
}

#else // defined(GLUE_OS_LINUX)

int64 Time::getHighResolutionTicks() noexcept
{
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);

    return (t.tv_sec * (int64) 1000000) + (t.tv_nsec / 1000);
}

int64 Time::getHighResolutionTicksPerSecond() noexcept
{
    return 1000000; // (microseconds)
}

double Time::getMillisecondCounterHiRes() noexcept
{
    return (double) getHighResolutionTicks() * 0.001;
}

#endif
GLUE_END_NAMESPACE
