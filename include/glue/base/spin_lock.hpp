//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/common.hpp"

#include <atomic>

namespace glue {

/**
    A simple spin-lock class that can be used as a simple, low-overhead mutex for
    uncontended situations.

    this type of lock is not re-entrant, and may
    be less efficient when used in a highly contended situation, but it's very small and
    requires almost no initialisation.
    It's most appropriate for simple situations where you're only going to hold the
    lock for a very brief time.

    This uses atomics, and thus may be heavier for systems with software
    atomics like older ARM architectures.
*/
class GLUE_API SpinLock final
{
public:
    SpinLock() = default;

    /** Acquires the lock.
        This will block until the lock has been successfully acquired by this thread.
        Note that a SpinLock is NOT re-entrant, and is not smart enough to know whether the
        caller thread already has the lock - so if a thread tries to acquire a lock that it
        already holds, this method will never return!

        It's strongly recommended that you never call this method directly - instead use the
        ScopedLockType class to manage the locking using an RAII pattern instead.
    */
    void lock() noexcept;

    /** Attempts to acquire the lock, return immediately true if this was successful. */
    bool try_lock() noexcept;

    /** Releases the lock. */
    void unlock() noexcept;

private:
    std::atomic<bool> m_flag{false};

    GLUE_DECLARE_NON_COPYABLE(SpinLock)
};

} // namespace glue
