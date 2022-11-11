//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/common.hpp"

#include <atomic>
#include <mutex>

namespace glue {

/// An Event is a synchronization object that
/// allows one thread to signal one or more
/// other threads that a certain event
/// has happened.
/// Usually, one thread signals an event,
/// while one or more other threads wait
/// for an event to become signalled.
class GLUE_API WaitableEvent final
{
public:
    explicit WaitableEvent();

    // Returns the value of the event's internal "notified" state.
    bool isNotified()
    {
        return hasBeenNotifiedInternal(&m_notified);
    }

    // Blocks the calling thread until the event's "notified" state is
    // `true`. Note that if `notify()` has been previously called on this
    // event, this function will immediately return.
    void wait();

    // Blocks until either the event's "notified" state is `true` (which
    // may occur immediately) or the timeout has elapsed, returning the value of
    // its "notified" state in either case.
    bool wait(int64 milliseconds);

    // Sets the "notified" state of this event to `true` and wakes waiting
    // threads. Note: do not call `notify()` multiple times on the same
    // `Event`; calling `notify()` more than once on the same event
    // results in undefined behavior.
    void set();

    void reset();

private:
    static inline bool hasBeenNotifiedInternal(const std::atomic<bool>* notified_yet)
    {
        return notified_yet->load(std::memory_order_acquire);
    }

    std::mutex m_mutex;
    std::condition_variable m_cond;
    std::atomic<bool> m_notified;

    GLUE_DECLARE_NON_COPYABLE(WaitableEvent)
};

} // namespace glue
