//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/base/barrier.h"

#include "glue/base/log.h"

namespace glue {

Barrier::Barrier(int num_threads) : m_num_threads(num_threads), m_num_started(0)
{}

void Barrier::wait()
{
    std::unique_lock<std::mutex> lk(m_mutex);

    if (++m_num_started >= m_num_threads)
        m_cond.notify_all();
    else
        m_cond.wait(lk);
}

void Barrier::reset()
{
    std::lock_guard<std::mutex> lk(m_mutex);
    m_num_started = 0;
}

} // namespace glue
