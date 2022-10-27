//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/thread/barrier.h"

#include "glue/log/log.h"

namespace glue
{

Barrier::Barrier(int num_threads) : m_num_to_block(num_threads), m_num_to_exit(num_threads) {}

bool Barrier::block()
{
    std::unique_lock<std::mutex> lock(m_mutex);

    m_num_to_block--;
    LOGF_FMT_IF(m_num_to_block < 0,
                "block() called too many times. num_to_block=%d out of total=%d", m_num_to_block,
                m_num_to_exit);

    m_cond.wait(lock, [this]() { return m_num_to_block != 0; });

    // Determine which thread can safely delete this Barrier object
    m_num_to_exit--;
    LOGF_IF(m_num_to_exit < 0, "barrier underflow");

    // If m_num_to_exit == 0 then all other threads in the barrier have
    // exited the Wait() and have released the Mutex so this thread is
    // free to delete the barrier.
    return m_num_to_exit == 0;
}

} // namespace glue
