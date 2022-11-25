//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/base/countdown_latch.h"

#include "glue/base/log.h"

namespace glue {

CountDownLatch::CountDownLatch(int initial_count)
    : m_done(initial_count == 0 ? true : false), m_num_waiting(0), m_count(initial_count) {
    if (initial_count < 0) {
        LogFatal("", "initial_count negative");
    }
}

bool CountDownLatch::countDown() noexcept {
    int count = m_count.fetch_sub(1, std::memory_order_acq_rel) - 1;
    if (count < 0) {
        LogFatal("", "called too many times");
    }
    if (count == 0) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_done = true;
        m_cond.notify_all();
        return true;
    }
    return false;
}

void CountDownLatch::wait() noexcept {
    std::unique_lock<std::mutex> lock(m_mutex);

    // only one thread may call wait(). To support more than one thread,
    // implement a counter num_to_exit, like in the Barrier class.
    if (m_num_waiting != 0) {
        LogFatal("", "multiple threads called wait()");
    }

    m_num_waiting++;

    m_cond.wait(lock, [this] { return m_done; });

    // At this point, we know that all threads executing countDown
    // will not touch this object again.
    // Therefore, the thread calling this method is free to delete the object
    // after we return from this method.
}

}  // namespace glue
