//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/scoped_lock.h"

GLUE_START_NAMESPACE

//==============================================================================

typedef std::mutex Mutex;

//==============================================================================
/**
    A class that can be used in place of a real Mutex object, but which
    doesn't perform any locking.

    This is currently used by some templated classes, and most compilers should
    manage to optimise it out of existence.

    @tags{Core}
*/
class GLUE_API EmptyMutex
{
public:
    using ScopedLock = ScopedLock<EmptyMutex>;

    inline EmptyMutex() = default;
    inline ~EmptyMutex() = default;

    inline void lock() noexcept
    {}
    inline bool try_lock() noexcept
    {
        return true;
    }
    inline void unlock() noexcept
    {}

private:
    GLUE_DECLARE_NON_COPYABLE(EmptyMutex)
};

GLUE_END_NAMESPACE
