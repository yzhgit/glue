//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/base/barrier.h"

#include <catch2/catch.hpp>

#include <chrono>
#include <thread>
#include <vector>

#include "glue/base/mutex.h"
#include "glue/base/scoped_lock.h"

namespace glue {
namespace test {

    TEST_CASE("Barrier SanityTest")
    {
        constexpr int kNumThreads = 10;
        Barrier barrier(kNumThreads);

        Mutex mutex;
        int counter = 0; // Guarded by mutex.

        auto thread_func = [&] {
            barrier.wait();

            // Increment the counter.
            ScopedLock<Mutex> lock(mutex);
            ++counter;
        };

        // Start (kNumThreads - 1) threads running thread_func.
        std::vector<std::thread> threads;
        for (int i = 0; i < kNumThreads - 1; ++i) { threads.push_back(std::thread(thread_func)); }

        // Give (kNumThreads - 1) threads a chance to reach the barrier.
        // This test assumes at least one thread will have run after the
        // sleep has elapsed. Sleeping in a test is usually bad form, but we
        // need to make sure that we are testing the barrier instead of some
        // other synchronization method.
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // The counter should still be zero since no thread should have
        // been able to pass the barrier yet.
        {
            ScopedLock<Mutex> lock(mutex);
            REQUIRE(counter == 0);
        }

        // Start 1 more thread. This should make all threads pass the barrier.
        threads.push_back(std::thread(thread_func));

        // All threads should now be able to proceed and finish.
        for (auto& thread : threads) { thread.join(); }

        // All threads should now have incremented the counter.
        ScopedLock<Mutex> lock(mutex);
        REQUIRE(counter == kNumThreads);
    }

} // namespace test
} // namespace glue
