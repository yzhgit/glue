//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/base/spin_lock.h"

#if defined(GLUE_COMPILER_MSVC)
    #include <windows.h>
#elif defined(GLUE_ARCH_X86)
    #if defined(GLUE_COMPILER_CLANG)
        #include <emmintrin.h>
    #endif
#endif

#if defined(HAVE_SCHED_YIELD)
    #include <sched.h>
#endif

namespace glue {

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
#if defined(GLUE_COMPILER_MSVC)
            YieldProcessor();
#elif defined(GLUE_ARCH_X86)
    #if defined(GLUE_COMPILER_CLANG)
            _mm_pause();
    #else
            __builtin_ia32_pause();
    #endif
#elif defined(GLUE_ARCH_ANY_ARM)
            // This intrinsic should fail to be found if YIELD is not supported on the current
            // processor.
            __yield();
#else
            // TODO: Issue PAGE/YIELD on other architectures.
            std::thread::yield();
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

void SpinLock::unlock() noexcept
{
    m_flag.store(false, std::memory_order_release);
}

}
