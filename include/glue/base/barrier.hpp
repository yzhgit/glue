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
// the `Barrier` by calling `wait()` on the barrier, which will block that
// thread; no call to `wait()` will return until `num_threads` threads have
// called it.
//
//
// Example:
//
//   // Main thread creates a `Barrier`:
//   barrier = new Barrier(num_threads);
//
//   // Each participating thread could then call:
//   barrier->wait();
//   delete barrier;
//
//
class GLUE_API Barrier final
{
public:
    // `num_threads` is the number of threads that will participate in the barrier
    explicit Barrier(int num_threads);

    // Barrier::wait()
    //
    // Blocks the current thread, and returns only when the `num_threads`
    // threshold of threads utilizing this barrier has been reached.
    void wait();

    void reset();

private:
    int m_num_threads;
    int m_num_started;
    std::mutex m_mutex;
    std::condition_variable m_cond;

    GLUE_DECLARE_NON_COPYABLE(Barrier)
};

} // namespace glue
