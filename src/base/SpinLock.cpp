
//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/base/SpinLock.h"

#if defined(_MSC_VER)
    #include <windows.h>
#elif defined(__i386__) || defined(__x86_64__)
    #if defined(__clang__)
        #include <emmintrin.h>
    #endif
#endif

/* if GCC on i386/x86_64 or if the built-in is present */
#if ((defined(__GNUC__) && !defined(__clang__)) && (defined(__i386__) || defined(__x86_64__))) ||  \
    GLUE_HAVE_BUILTIN(__builtin_ia32_pause)
    #define HAVE_BUILTIN_IA32_PAUSE 1
#else
    #define HAVE_BUILTIN_IA32_PAUSE 0
#endif

#if defined(HAVE_SCHED_YIELD)
    #include <sched.h>
#endif

namespace glue
{

void SpinLock::lock() noexcept
{
    for (;;)
    {
        // Optimistically assume the lock is free on the first try
        if (!m_flag.exchange(true, std::memory_order_acquire)) { return; }
        // Wait for lock to be released without generating cache misses
        // Reduce cache coherency traffic
        while (m_flag.load(std::memory_order_relaxed))
        {
// Issue a Pause/Yield instruction while spinning.
#if defined(_MSC_VER)
            YieldProcessor();
#elif defined(__i386__) || defined(__x86_64__)
    #if defined(__clang__)
            _mm_pause();
    #else
            __builtin_ia32_pause();
    #endif
#elif defined(__arm__)
            // This intrinsic should fail to be found if YIELD is not supported on the current
            // processor.
            __yield();
#else
            // TODO: Issue PAGE/YIELD on other architectures.
#endif
        }
    }
}

bool SpinLock::try_lock() noexcept
{
    // First do a relaxed load to check if lock is free in order to prevent
    // unnecessary cache misses if someone does while(!try_lock())
    return !m_flag.load(std::memory_order_relaxed) &&
           !m_flag.exchange(true, std::memory_order_acquire);
}

void SpinLock::unlock() noexcept { m_flag.store(false, std::memory_order_release); }

} // namespace glue
