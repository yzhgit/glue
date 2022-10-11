//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/thread/event.h"

#include <chrono>

namespace glue
{

Event::Event() : m_notified(false) {}

void Event::notify()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_notified = true;
    m_cond.notify_all();
}

void Event::wait() const
{
    if (!hasBeenNotifiedInternal(&m_notified))
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond.wait(lock, [this]() { return m_notified.load(std::memory_order_acquire); });
    }
}

bool Event::wait(long milliseconds) const
{
    bool notified = hasBeenNotifiedInternal(&m_notified);
    if (!notified)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        notified = m_cond.wait_for(lock, std::chrono::milliseconds(milliseconds),
                                   [this]() { return m_notified.load(std::memory_order_acquire); });
    }

    return notified;
}

} // namespace glue
