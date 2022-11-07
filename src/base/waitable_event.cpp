//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/base/WaitableEvent.h"

#include <chrono>

GLUE_START_NAMESPACE

WaitableEvent::WaitableEvent() : m_notified(false) {}

void WaitableEvent::wait() const
{
    if (!hasBeenNotifiedInternal(&m_notified))
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond.wait(lock, [this]() { return m_notified.load(std::memory_order_acquire); });
    }
}

bool WaitableEvent::wait(int64 milliseconds) const
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

void WaitableEvent::set()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_notified = true;
    m_cond.notify_all();
}

void WaitableEvent::reset()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_notified = false;
}

GLUE_END_NAMESPACE
