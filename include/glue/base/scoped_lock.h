//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/common.h"

namespace glue {

//==============================================================================
/**
    Automatically locks and unlocks a mutex object.

    Use one of these as a local variable to provide RAII-based locking of a
   mutex.

    The templated class could be a Mutex, SpinLock, or anything else that
    provides lock() and unlock() methods.

    e.g. @code
    Mutex mutex;

    for (;;)
    {
        const ScopedLock<Mutex> myScopedLock (mutex);
        // mutex is now locked

        ...do some stuff...

        // mutex gets unlocked here.
    }
    @endcode

    @see ScopedUnlock, Mutex, SpinLock, ScopedLock, ScopedUnlock

    @tags{Core}
*/
template <class LockType>
class ScopedLock {
   public:
    //==============================================================================
    /** Creates a ScopedLock.

        As soon as it is created, this will acquire the lock, and when the
       ScopedLock object is deleted, the lock will be released.

        Make sure this object is created and deleted by the same thread,
        otherwise there are no guarantees what will happen! Best just to use it
        as a local stack object, rather than creating one with the new()
       operator.
    */
    inline explicit ScopedLock(LockType& lock) noexcept : m_lock(lock) { m_lock.lock(); }

    /** Destructor.
        The lock will be released when the destructor is called.
        Make sure this object is created and deleted by the same thread,
       otherwise there are no guarantees what will happen!
    */
    inline ~ScopedLock() noexcept { m_lock.unlock(); }

   private:
    //==============================================================================
    LockType& m_lock;

    GLUE_DECLARE_NON_COPYABLE(ScopedLock)
};

//==============================================================================
/**
    Automatically unlocks and re-locks a mutex object.

    This is the reverse of a ScopedLock object - instead of locking the mutex
    for the lifetime of this object, it unlocks it.

    Make sure you don't try to unlock mutexes that aren't actually locked!

    e.g. @code

    Mutex mutex;

    for (;;)
    {
        const ScopedLock<Mutex> myScopedLock (mutex);
        // mutex is now locked

        ... do some stuff with it locked ..

        while (xyz)
        {
            ... do some stuff with it locked ..

            const ScopedUnlock<Mutex> unlocker (mutex);

            // mutex is now unlocked for the remainder of this block,
            // and re-locked at the end.

            ...do some stuff with it unlocked ...
        }

        // mutex gets unlocked here.
    }
    @endcode

    @see ScopedLock, Mutex, ScopedLock, ScopedUnlock

    @tags{Core}
*/
template <class LockType>
class ScopedUnlock {
   public:
    //==============================================================================
    /** Creates a ScopedUnlock.

        As soon as it is created, this will unlock the Mutex, and
        when the ScopedLock object is deleted, the Mutex will
        be re-locked.

        Make sure this object is created and deleted by the same thread,
        otherwise there are no guarantees what will happen! Best just to use it
        as a local stack object, rather than creating one with the new()
       operator.
    */
    inline explicit ScopedUnlock(LockType& lock) noexcept : m_lock(lock) { lock.unlock(); }

    /** Destructor.

        The Mutex will be unlocked when the destructor is called.

        Make sure this object is created and deleted by the same thread,
        otherwise there are no guarantees what will happen!
    */
    inline ~ScopedUnlock() noexcept { m_lock.lock(); }

   private:
    //==============================================================================
    LockType& m_lock;

    GLUE_DECLARE_NON_COPYABLE(ScopedUnlock)
};

}  // namespace glue
