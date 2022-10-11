//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/log/log.h"
#include "glue/thread/thread.h"

#include <windows.h>

namespace glue
{

Thread::Thread() : m_id(uniqueId()), m_name(makeName()) {}

Thread::Thread(const std::string& name) : m_id(uniqueId()), m_name(name) {}

std::string Thread::makeName()
{
    std::ostringstream name;
    name << '#' << m_id;
    return name.str();
}

unsigned int Thread::uniqueId()
{
    static std::atomic<unsigned int> counter;
    return ++counter;
}

std::string Thread::name() const { return m_name; }

void Thread::setStackSize(unsigned int size) {}

unsigned int Thread::stackSize() const { return m_stackSize; }

Thread::~Thread()
{
    if (m_thread) CloseHandle(m_thread);
}

void Thread::setPriority(Priority prio)
{
    if (prio != m_priority)
    {
        m_priority = prio;
        if (m_thread)
        {
            if (SetThreadPriority(m_thread, m_priority) == 0)
            {
                LOGW << "cannot set thread priority";
            }
        }
    }
}

void Thread::create(Entry ent, void* pData)
{
#if defined(_DLL)
    m_thread = CreateThread(NULL, m_stackSize, ent, pData, 0, &m_threadId);
#else
    unsigned threadId;
    m_thread = (HANDLE) _beginthreadex(NULL, m_stackSize, ent, this, 0, &threadId);
    m_threadId = static_cast<DWORD>(threadId);
#endif
    if (!m_thread) throw SystemException("cannot create thread");
    if (m_priority != PRIO_NORMAL_IMPL && !SetThreadPriority(m_thread, m_priority))
        throw SystemException("cannot set thread priority");
}

void Thread::join()
{
    if (!m_thread) return;

    switch (WaitForSingleObject(m_thread, INFINITE))
    {
    case WAIT_OBJECT_0:
        threadCleanup();
        return;
    default:
        throw SystemException("cannot join thread");
    }
}

bool Thread::join(long milliseconds)
{
    if (!m_thread) return true;

    switch (WaitForSingleObject(m_thread, milliseconds + 1))
    {
    case WAIT_TIMEOUT:
        return false;
    case WAIT_OBJECT_0:
        threadCleanup();
        return true;
    default:
        throw SystemException("cannot join thread");
    }
}

bool Thread::isRunning() const
{
    if (m_thread)
    {
        DWORD ec = 0;
        return GetExitCodeThread(m_thread, &ec) && ec == STILL_ACTIVE;
    }
    return false;
}

void Thread::threadCleanup()
{
    if (!m_thread) return;
    if (CloseHandle(m_thread)) m_thread = 0;
}

Thread* Thread::current() { return _currentThreadHolder.get(); }

Thread::TID Thread::currentTid() { return GetCurrentThreadId(); }

#if defined(_DLL)
DWORD WINAPI Thread::runnableEntry(LPVOID pThread)
#else
unsigned __stdcall Thread::runnableEntry(void* pThread)
#endif
{
    _currentThreadHolder.set(reinterpret_cast<Thread*>(pThread));
#if defined(POCO_WIN32_DEBUGGER_THREAD_NAMES)
    setThreadName(-1, reinterpret_cast<Thread*>(pThread)->getName().c_str());
#endif
    try
    {
        reinterpret_cast<Thread*>(pThread)->_pRunnableTarget->run();
    } catch (Exception& exc)
    {
        ErrorHandler::handle(exc);
    } catch (std::exception& exc)
    {
        ErrorHandler::handle(exc);
    } catch (...)
    {
        ErrorHandler::handle();
    }
    return 0;
}

} // namespace glue
