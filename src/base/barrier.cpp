//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/base/barrier.h"

#include "glue/base/log.h"

GLUE_START_NAMESPACE

Barrier::Barrier(int num_threads) : m_num_to_block(num_threads), m_num_to_exit(num_threads)
{}

bool Barrier::block()
{
    std::unique_lock<std::mutex> lock(m_mutex);

    m_num_to_block--;
    if (m_num_to_exit < 0)
    {
        LogFatal("", "block() called too many times. num_to_block=%d out of total=%d",
                 m_num_to_block, m_num_to_exit);
    }

    m_cond.wait(lock, [this]() { return m_num_to_block != 0; });

    // Determine which thread can safely delete this Barrier object
    m_num_to_exit--;
    if (m_num_to_exit < 0) { LogFatal() << "barrier underflow"; }

    // If m_num_to_exit == 0 then all other threads in the barrier have
    // exited the Wait() and have released the mutex so this thread is
    // free to delete the barrier.
    return m_num_to_exit == 0;
}

GLUE_END_NAMESPACE
