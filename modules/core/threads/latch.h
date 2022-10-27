//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/precompiled.h"

#include <atomic>
#include <condition_variable>
#include <mutex>

namespace glue
{

// Latch
//
// This class allows a thread to block for a pre-specified number of actions.
// `Latch` maintains a single non-negative abstract integer "count"
// with an initial value `initial_count`. A thread can then call `wait()` on
// this blocking counter to block until the specified number of events occur;
// worker threads then call 'countDown()` on the counter upon completion of
// their work. Once the counter's internal "count" reaches zero, the blocked
// thread unblocks.
//
// A `Latch` requires the following:
//     - its `initial_count` is non-negative.
//     - the number of calls to `countDown()` on it is at most
//       `initial_count`.
//     - `wait()` is called at most once on it.
//
// Given the above requirements, a `Latch` provides the following
// guarantees:
//     - Once its internal "count" reaches zero, no legal action on the object
//       can further change the value of "count".
//     - When `wait()` returns, it is legal to destroy the `Latch`.
//     - When `wait()` returns, the number of calls to `countDown()` on
//       this blocking counter exactly equals `initial_count`.
//
// Example:
//     Latch bcount(N);         // there are N items of work
//     ... Allow worker threads to start.
//     ... On completing each work item, workers do:
//     ... bcount.countDown();      // an item of work has been completed
//
//     bcount.wait();                    // wait for all work to be complete
//
class GLUE_API Latch final
{
public:
    explicit Latch(int initial_count);

    // Latch::countDown()
    //
    // Decrements the counter's "count" by one, and return "count == 0". This
    // function requires that "count != 0" when it is called.
    //
    // Memory ordering: For any threads X and Y, any action taken by X
    // before it calls `countDown()` is visible to thread Y after
    // Y's call to `countDown()`, provided Y's call returns `true`.
    bool countDown() noexcept;

    // Latch::wait()
    //
    // Blocks until the counter reaches zero. This function may be called at most
    // once. On return, `countDown()` will have been called "initial_count"
    // times and the blocking counter may be destroyed.
    //
    // Memory ordering: For any threads X and Y, any action taken by X
    // before X calls `countDown()` is visible to Y after Y returns
    // from `wait()`.
    void wait() noexcept;

private:
    bool m_done;
    int m_num_waiting;
    std::atomic<int> m_count;
    std::mutex m_mutex;
    std::condition_variable m_cond;

    GLUE_DECLARE_NON_COPYABLE(Latch)
};

} // namespace glue
