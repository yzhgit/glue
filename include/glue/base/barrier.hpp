//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/common.hpp"

#include <condition_variable>
#include <mutex>

namespace glue {

// Barrier
//
// This class creates a barrier which blocks threads until a prespecified
// threshold of threads (`num_threads`) utilizes the barrier. A thread utilizes
// the `Barrier` by calling `block()` on the barrier, which will block that
// thread; no call to `block()` will return until `num_threads` threads have
// called it.
//
// Exactly one call to `block()` will return `true`, which is then responsible
// for destroying the barrier; because stack allocation will cause the barrier
// to be deleted when it is out of scope, barriers should not be stack
// allocated.
//
// Example:
//
//   // Main thread creates a `Barrier`:
//   barrier = new Barrier(num_threads);
//
//   // Each participating thread could then call:
//   if (barrier->block()) delete barrier;  // Exactly one call to `block()`
//                                          // returns `true`; that call
//                                          // deletes the barrier.
class GLUE_API Barrier final
{
public:
    // `num_threads` is the number of threads that will participate in the barrier
    explicit Barrier(int num_threads);

    // Barrier::block()
    //
    // Blocks the current thread, and returns only when the `num_threads`
    // threshold of threads utilizing this barrier has been reached. `block()`
    // returns `true` for precisely one caller, which may then destroy the
    // barrier.
    //
    // Memory ordering: For any threads X and Y, any action taken by X
    // before X calls `block()` will be visible to Y after Y returns from
    // `block()`.
    bool block();

private:
    int m_num_to_block;
    int m_num_to_exit;
    std::mutex m_mutex;
    std::condition_variable m_cond;

    GLUE_DECLARE_NON_COPYABLE(Barrier)
};

} // namespace glue
