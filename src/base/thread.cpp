//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/base/thread.h"

#include "glue/base/log.h"

GLUE_START_NAMESPACE

class RunnableHolder : public Runnable
{
public:
    RunnableHolder(Runnable& target) : m_target(target) {}

    ~RunnableHolder() {}

    void run() { m_target.run(); }

private:
    Runnable& m_target;
};

Thread::Thread() : m_threadRunning(false), m_threadDone(true), m_name("") {}

Thread::Thread(const std::string& name)
    : m_threadRunning(false), m_threadDone(true)
{
    setName(name);
}

//-------------------------------------------------
bool Thread::isRunning() const { return m_threadRunning; }

//-------------------------------------------------
std::thread::id Thread::getThreadId() const { return m_thread.get_id(); }

//-------------------------------------------------
std::string Thread::getName() const { return m_name; }

//-------------------------------------------------
void Thread::setName(const std::string& name) { this->m_name = name; }

//-------------------------------------------------
void Thread::start(Runnable& target) { start(std::make_shared<RunnableHolder>(target)); }

void Thread::start(std::shared_ptr<Runnable> pTarget)
{
    std::unique_lock<std::mutex> lck(m_mutex);
    if (m_threadRunning || !m_threadDone)
    {
        LogWarn("Thread") << "- name: " << getName() << " - Cannot start, thread already running.";
        return;
    }

    m_threadDone = false;
    m_threadRunning = true;
    m_pRunnableTarget = pTarget;

    m_thread = std::thread(std::bind(&Thread::threadEntry, this));
}

//-------------------------------------------------
void Thread::stop() { m_threadRunning = false; }

//-------------------------------------------------
void Thread::waitForThread(bool callStopThread, long milliseconds)
{
    if (!m_threadDone)
    {
        // tell thread to stop
        if (callStopThread) { stop(); }

        // wait for the thread to finish
        if (isCurrentThread())
        {
            return; // waitForThread should only be called outside thread
        }

        if (INFINITE_JOIN_TIMEOUT == milliseconds)
        {
            std::unique_lock<std::mutex> lck(m_mutex);
            if (!m_threadDone) { m_condition.wait(lck); }
        }
        else
        {
            std::unique_lock<std::mutex> lck(m_mutex);
            if (!m_threadDone &&
                m_condition.wait_for(lck, std::chrono::milliseconds(milliseconds)) ==
                    std::cv_status::timeout)
            {
                // unable to completely wait for thread
            }
        }
    }
}

//-------------------------------------------------
void Thread::sleep(long milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

//-------------------------------------------------
void Thread::yield() { std::this_thread::yield(); }

//-------------------------------------------------
bool Thread::isCurrentThread() const { return std::this_thread::get_id() == m_thread.get_id(); }

//-------------------------------------------------
std::thread& Thread::getNativeThread() { return m_thread; }

//-------------------------------------------------
const std::thread& Thread::getNativeThread() const { return m_thread; }

//-------------------------------------------------
void Thread::threadEntry()
{
    try
    {
        m_pRunnableTarget->run();
    } catch (const std::exception& exc)
    {
        LogFatal() << exc.what();
    } catch (...)
    {
        LogFatal() << "Unknown exception.";
    }
    try
    {
        m_thread.detach();
    } catch (...)
    {
    }

    std::unique_lock<std::mutex> lck(m_mutex);
    m_threadRunning = false;
    m_threadDone = true;
    m_condition.notify_all();
}

GLUE_END_NAMESPACE
