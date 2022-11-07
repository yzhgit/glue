//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/base/thread_pool.h"

#include "glue/base/exception.h"
#include "glue/base/log.h"
#include "glue/base/thread.h"
#include "glue/base/waitable_event.h"

GLUE_START_NAMESPACE

class PooledThread : public Runnable
{
public:
    PooledThread(const std::string& name);
    ~PooledThread();

    void start();
    void start(Runnable& target);
    void start(Runnable& target, const std::string& name);
    bool idle();
    int idleTime();
    void join();
    void activate();
    void release();
    void run();

private:
    volatile bool m_idle;
    volatile std::time_t m_idleTime;
    Runnable* m_pTarget;
    std::string m_name;
    Thread m_thread;
    WaitableEvent m_targetReady;
    WaitableEvent m_targetCompleted;
    WaitableEvent m_started;
    FastMutex m_mutex;
};

PooledThread::PooledThread(const std::string& name)
    : m_idle(true), m_idleTime(0), m_pTarget(0), m_name(name), m_thread(name)
{
    m_idleTime = std::time(NULL);
}

PooledThread::~PooledThread()
{}

void PooledThread::start()
{
    m_thread.start(*this);
    m_started.wait();
}

void PooledThread::start(Runnable& target)
{
    FastMutex::ScopedLock lock(m_mutex);

    GLUE_ASSERT(m_pTarget == 0);

    m_pTarget = &target;
    m_targetReady.set();
}

void PooledThread::start(Runnable& target, const std::string& name)
{
    FastMutex::ScopedLock lock(m_mutex);

    std::string fullName(name);
    if (name.empty()) { fullName = m_name; }
    else
    {
        fullName.append(" (");
        fullName.append(m_name);
        fullName.append(")");
    }
    m_thread.setName(fullName);

    GLUE_ASSERT(m_pTarget == 0);

    m_pTarget = &target;
    m_targetReady.set();
}

inline bool PooledThread::idle()
{
    FastMutex::ScopedLock lock(m_mutex);
    return m_idle;
}

int PooledThread::idleTime()
{
    FastMutex::ScopedLock lock(m_mutex);

    return (int) (time(NULL) - m_idleTime);
}

void PooledThread::join()
{
    m_mutex.lock();
    Runnable* pTarget = m_pTarget;
    m_mutex.unlock();
    if (pTarget) m_targetCompleted.wait();
}

void PooledThread::activate()
{
    FastMutex::ScopedLock lock(m_mutex);

    GLUE_ASSERT(m_idle);
    m_idle = false;
    m_targetCompleted.reset();
}

void PooledThread::release()
{
    const long JOIN_TIMEOUT = 10000;

    m_mutex.lock();
    m_pTarget = 0;
    m_mutex.unlock();
    // In case of a statically allocated thread pool (such
    // as the default thread pool), Windows may have already
    // terminated the thread before we got here.
    if (m_thread.isRunning()) m_targetReady.set();

    m_thread.waitForThread(true, JOIN_TIMEOUT);
    // delete this;
}

void PooledThread::run()
{
    m_started.set();
    for (;;)
    {
        m_targetReady.wait();
        m_mutex.lock();
        if (m_pTarget) // a NULL target means kill yourself
        {
            Runnable* pTarget = m_pTarget;
            m_mutex.unlock();
            try
            {
                pTarget->run();
            } catch (std::exception& exc)
            {
                // ErrorHandler::handle(exc);
            } catch (...)
            {
                // ErrorHandler::handle();
            }
            FastMutex::ScopedLock lock(m_mutex);
            m_pTarget = 0;
            m_idleTime = time(NULL);

            m_idle = true;
            m_targetCompleted.set();
            m_thread.setName(m_name);
        }
        else
        {
            m_mutex.unlock();
            break;
        }
    }
}

ThreadPool::ThreadPool(int minCapacity, int maxCapacity, int idleTime)
    : m_minCapacity(minCapacity)
    , m_maxCapacity(maxCapacity)
    , m_idleTime(idleTime)
    , m_serial(0)
    , m_age(0)
{
    GLUE_ASSERT(minCapacity >= 1 && maxCapacity >= minCapacity && idleTime > 0);

    for (int i = 0; i < m_minCapacity; i++)
    {
        PooledThread* pThread = createThread();
        m_threads.push_back(pThread);
        pThread->start();
    }
}

ThreadPool::ThreadPool(const std::string& name, int minCapacity, int maxCapacity, int idleTime)
    : m_name(name)
    , m_minCapacity(minCapacity)
    , m_maxCapacity(maxCapacity)
    , m_idleTime(idleTime)
    , m_serial(0)
    , m_age(0)
{
    GLUE_ASSERT(minCapacity >= 1 && maxCapacity >= minCapacity && idleTime > 0);

    for (int i = 0; i < m_minCapacity; i++)
    {
        PooledThread* pThread = createThread();
        m_threads.push_back(pThread);
        pThread->start();
    }
}

ThreadPool::~ThreadPool()
{
    try
    {
        stopAll();
    } catch (...)
    {
        LogWarn() << "~ThreadPool exception";
    }
}

void ThreadPool::addCapacity(int n)
{
    FastMutex::ScopedLock lock(m_mutex);

    GLUE_ASSERT(m_maxCapacity + n >= m_minCapacity);
    m_maxCapacity += n;
    housekeep();
}

int ThreadPool::capacity() const
{
    FastMutex::ScopedLock lock(m_mutex);
    return m_maxCapacity;
}

int ThreadPool::available() const
{
    FastMutex::ScopedLock lock(m_mutex);

    int count = 0;
    for (auto pThread : m_threads)
    {
        if (pThread->idle()) ++count;
    }
    return (int) (count + m_maxCapacity - m_threads.size());
}

int ThreadPool::used() const
{
    FastMutex::ScopedLock lock(m_mutex);

    int count = 0;
    for (auto pThread : m_threads)
    {
        if (!pThread->idle()) ++count;
    }
    return count;
}

int ThreadPool::allocated() const
{
    FastMutex::ScopedLock lock(m_mutex);

    return int(m_threads.size());
}

void ThreadPool::start(Runnable& target)
{
    getThread()->start(target);
}

void ThreadPool::start(Runnable& target, const std::string& name)
{
    getThread()->start(target, name);
}

void ThreadPool::stopAll()
{
    FastMutex::ScopedLock lock(m_mutex);

    for (auto pThread : m_threads) { pThread->release(); }
    m_threads.clear();
}

void ThreadPool::joinAll()
{
    FastMutex::ScopedLock lock(m_mutex);

    for (auto pThread : m_threads) { pThread->join(); }
    housekeep();
}

void ThreadPool::collect()
{
    FastMutex::ScopedLock lock(m_mutex);
    housekeep();
}

void ThreadPool::housekeep()
{
    m_age = 0;
    if (m_threads.size() <= m_minCapacity) return;

    ThreadVec idleThreads;
    ThreadVec expiredThreads;
    ThreadVec activeThreads;
    idleThreads.reserve(m_threads.size());
    activeThreads.reserve(m_threads.size());

    for (auto pThread : m_threads)
    {
        if (pThread->idle())
        {
            if (pThread->idleTime() < m_idleTime)
                idleThreads.push_back(pThread);
            else
                expiredThreads.push_back(pThread);
        }
        else
            activeThreads.push_back(pThread);
    }
    int n = (int) activeThreads.size();
    int limit = (int) idleThreads.size() + n;
    if (limit < m_minCapacity) limit = m_minCapacity;
    idleThreads.insert(idleThreads.end(), expiredThreads.begin(), expiredThreads.end());
    m_threads.clear();
    for (auto pIdle : idleThreads)
    {
        if (n < limit)
        {
            m_threads.push_back(pIdle);
            ++n;
        }
        else
            pIdle->release();
    }
    m_threads.insert(m_threads.end(), activeThreads.begin(), activeThreads.end());
}

PooledThread* ThreadPool::getThread()
{
    FastMutex::ScopedLock lock(m_mutex);

    if (++m_age == 32) housekeep();

    PooledThread* pThread = 0;
    for (ThreadVec::iterator it = m_threads.begin(); !pThread && it != m_threads.end(); ++it)
    {
        if ((*it)->idle()) pThread = *it;
    }
    if (!pThread)
    {
        if (m_threads.size() < m_maxCapacity)
        {
            pThread = createThread();
            try
            {
                pThread->start();
                m_threads.push_back(pThread);
            } catch (...)
            {
                delete pThread;
                throw;
            }
        }
        else
            throw NoThreadAvailableException();
    }
    pThread->activate();
    return pThread;
}

PooledThread* ThreadPool::createThread()
{
    std::ostringstream name;
    name << m_name << "[#" << ++m_serial << "]";
    return new PooledThread(name.str());
}

class ThreadPoolSingletonHolder
{
public:
    ThreadPoolSingletonHolder()
    {
        _pPool = 0;
    }
    ~ThreadPoolSingletonHolder()
    {
        delete _pPool;
    }
    ThreadPool* pool()
    {
        FastMutex::ScopedLock lock(_mutex);

        if (!_pPool) { _pPool = new ThreadPool("default"); }
        return _pPool;
    }

private:
    ThreadPool* _pPool;
    FastMutex _mutex;
};

namespace {
static ThreadPoolSingletonHolder sh;
}

ThreadPool& ThreadPool::defaultPool()
{
    return *sh.pool();
}

GLUE_END_NAMESPACE
