//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "threads/ScopedLock.h"

namespace glue
{

//==============================================================================
/**
    A re-entrant mutex.

    A Mutex acts as a re-entrant mutex object. The best way to lock and unlock
    one of these is by using RAII in the form of a local ScopedLock object - have a look
    through the codebase for many examples of how to do this.

    In almost all cases you'll want to declare your Mutex as a member variable.
    Occasionally you may want to declare one as a static variable, but in that case the usual
    C++ static object order-of-construction warnings should be heeded.

    @see ScopedLock, ScopedUnlock, SpinLock, ReadWriteLock, Thread

    @tags{Core}
*/
class GLUE_API Mutex : public std::recursive_mutex
{
public:
    using ScopedLock = ScopedLock<Mutex>;

private:
    GLUE_DECLARE_NON_COPYABLE(Mutex)
};

class GLUE_API FastMutex : public std::mutex
{
public:
    using ScopedLock = ScopedLock<FastMutex>;

private:
    GLUE_DECLARE_NON_COPYABLE(FastMutex)
};

//==============================================================================
/**
    A class that can be used in place of a real Mutex object, but which
    doesn't perform any locking.

    This is currently used by some templated classes, and most compilers should
    manage to optimise it out of existence.

    @see Mutex, Array, OwnedArray, ReferenceCountedArray

    @tags{Core}
*/
class GLUE_API EmptyMutex
{
public:
    using ScopedLock = ScopedLock<FastMutex>;

    inline EmptyMutex() = default;
    inline ~EmptyMutex() = default;

    inline void lock() noexcept {}
    inline bool try_lock() noexcept { return true; }
    inline void unlock() noexcept {}

private:
    GLUE_DECLARE_NON_COPYABLE(EmptyMutex)
};

} // namespace glue
