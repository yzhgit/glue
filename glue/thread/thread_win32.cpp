//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/log/log.h"
#include "glue/thread/thread.h"

#include <process.h>
#include <windows.h>

namespace
{
/// See <http://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx>
/// and <http://blogs.msdn.com/b/stevejs/archive/2005/12/19/505815.aspx> for
/// more information on the code below.

const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push, 8)
typedef struct tagTHREADNAME_INFO
{
    DWORD dwType;     // Must be 0x1000.
    LPCSTR szName;    // Pointer to name (in user addr space).
    DWORD dwThreadID; // Thread ID (-1=caller thread).
    DWORD dwFlags;    // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

void setThreadName(DWORD dwThreadID, const char* threadName)
{
    THREADNAME_INFO info;
    info.dwType = 0x1000;
    info.szName = threadName;
    info.dwThreadID = dwThreadID;
    info.dwFlags = 0;

    __try
    {
        RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*) &info);
    } __except (EXCEPTION_CONTINUE_EXECUTION)
    {
    }
}

} // namespace

namespace glue
{

struct Thread::ThreadData
{
    unsigned int id;
    unsigned int stackSize;
    Priority priority;
    std::string name;
    HANDLE threadHandle;
    unsigned int threadId;
    std::mutex mutex;
};

#if defined(_DLL)
DWORD WINAPI runnableEntry(LPVOID userData)
#else
unsigned __stdcall runnableEntry(void* userData)
#endif
{
    Thread* pThread = reinterpret_cast<Thread*>(userData);
#if defined(POCO_WIN32_DEBUGGER_THREAD_NAMES)
    setThreadName(-1, pThread->getName().c_str());
#endif
    try
    {
        pThread->run();
    } catch (std::exception& exc)
    {
    } catch (...)
    {
    }
    return 0;
}

Thread::Thread() : d_ptr(std::make_unique<ThreadData>())
{
    d_ptr->id = uniqueId();
    d_ptr->name = makeName();
}

Thread::Thread(const std::string& name) : d_ptr(std::make_unique<ThreadData>())
{
    d_ptr->id = uniqueId();
    d_ptr->name = name;
}

std::string Thread::makeName()
{
    std::ostringstream name;
    name << '#' << d_ptr->id;
    return name.str();
}

unsigned int Thread::uniqueId()
{
    static std::atomic<unsigned int> counter;
    return ++counter;
}

std::string Thread::name() const { return d_ptr->name; }

void Thread::setStackSize(unsigned int size) { d_ptr->stackSize = size; }

unsigned int Thread::stackSize() const { return d_ptr->stackSize; }

Thread::~Thread()
{
    if (d_ptr->threadHandle) CloseHandle(d_ptr->threadHandle);
}

void Thread::setPriority(Priority prio)
{
    if (prio != d_ptr->priority)
    {
        d_ptr->priority = prio;
        if (d_ptr->threadHandle)
        {
            if (SetThreadPriority(d_ptr->threadHandle, d_ptr->priority) == 0)
            {
                LOGW << "cannot set thread priority";
            }
        }
    }
}

void Thread::start(Priority priority)
{
    if (isRunning())
    {
        LOGW << "thread already running";
        return;
    }

#if defined(_DLL)
    // MSVC -MD or -MDd or MinGW build
    d_ptr->threadHandle =
        CreateThread(NULL, d_ptr->stackSize, runnableEntry, this, CREATE_SUSPENDED,
                     reinterpret_cast<LPDWORD>(&d_ptr->threadId));
#else
    // MSVC -MT or -MTd build
    d_ptr->threadHandle = (HANDLE) _beginthreadex(NULL, d_ptr->stackSize, runnableEntry, this,
                                                  CREATE_SUSPENDED, &d_ptr->threadId);
#endif
    if (!d_ptr->threadHandle)
    {
        LOGW << "cannot create thread";
        return;
    }

    if (d_ptr->priority != PRIO_NORMAL_IMPL &&
        !SetThreadPriority(d_ptr->threadHandle, d_ptr->priority))
    {
        LOGW << "cannot set thread priority";
        return;
    }
}

bool Thread::wait(unsigned long milliseconds)
{
    if (!d_ptr->threadHandle) return true;

    switch (WaitForSingleObject(d_ptr->threadHandle,
                                (milliseconds == ULONG_MAX) ? INFINITE : milliseconds + 1))
    {
    case WAIT_TIMEOUT:
        return false;
    case WAIT_OBJECT_0:
        threadCleanup();
        return true;
    default:
        return false;
    }
}

bool Thread::isRunning() const
{
    if (d_ptr->threadHandle)
    {
        DWORD ec = 0;
        return GetExitCodeThread(d_ptr->threadHandle, &ec) && ec == STILL_ACTIVE;
    }
    return false;
}

void Thread::threadCleanup()
{
    if (!d_ptr->threadHandle) return;
    if (CloseHandle(d_ptr->threadHandle)) d_ptr->threadHandle = 0;
}

Thread* Thread::currentThread() { return _currentThreadHolder.get(); }

Thread::ThreadID Thread::currentThreadId() { return GetCurrentThreadId(); }

} // namespace glue
