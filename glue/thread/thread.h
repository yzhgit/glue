//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/precompiled.h"

namespace glue
{

/**
    Encapsulates a thread.

    Subclasses derive from Thread and implement the run() method, in which they
    do their business. The thread can then be started with the start() method
    and controlled with various other methods.

    This class also contains some thread-related static methods, such
    as sleep(), yield(), getCurrentThreadId() etc.

    @see CriticalSection, WaitableEvent, Process, ThreadWithProgressWindow,
         MessageManagerLock

    @tags{Core}
*/
class GLUE_API Thread
{
public:
    typedef void* ThreadID;

    enum Priority
    {
        IdlePriority,

        LowestPriority,
        LowPriority,
        NormalPriority,
        HighPriority,
        HighestPriority,

        TimeCriticalPriority,

        InheritPriority
    };

    /**
        Creates a thread.

        When first created, the thread is not running. Use the start()
        method to start it.
    */
    Thread();

    explicit Thread(const std::string& name);

    /** Destructor.

        You must never attempt to delete a Thread object while it's still running -
        always call stop() and make sure your thread has stopped before deleting
        the object. Failing to do so will throw an assertion, and put you firmly into
        undefined behaviour territory.
    */
    virtual ~Thread();

    /// Returns the name of the thread.
    std::string name() const;

    /** Changes the thread's priority.

        May return false if for some reason the priority can't be changed.

        @param priority     the new priority, in the range 0 (lowest) to 10 (highest). A priority
                            of 5 is normal.
        @see realtimeAudioPriority
    */
    void setPriority(Priority priority);

    /** Returns the priority for a running thread.

      If the thread is not running, this function returns InheritPriority.
     */
    Priority priority() const;

    /// Sets the thread's stack size in bytes.
    /// Setting the stack size to 0 will use the default stack size.
    /// Typically, the real stack size is rounded up to the nearest
    /// page size multiple.
    void setStackSize(unsigned size);

    /// Returns the thread's stack size in bytes.
    /// If the default stack size is used, 0 is returned.
    unsigned int stackSize() const;

    /** Returns true if the thread is currently active */
    bool isRunning() const;

    /// Starts the thread with the given target.
    ///
    /// Note that the given Runnable object must remain
    /// valid during the entire lifetime of the thread, as
    /// only a reference to it is stored internally.
    void start(Priority priority = InheritPriority);

    /** Attempts to stop the thread running.

        This method will cause the threadShouldExit() method to return true
        and call notify() in case the thread is currently waiting.

        Hopefully the thread will then respond to this by exiting cleanly, and
        the stop method will wait for a given time-period for this to
        happen.

        If the thread is stuck and fails to respond after the timeout, it gets
        forcibly killed, which is a very bad thing to happen, as it could still
        be holding locks, etc. which are needed by other parts of your program.
    */
    void terminate();

    /** Waits for the thread to stop.
        This will wait until isRunning() is false or until a timeout expires.

        @param timeOutMilliseconds  the time to wait, in milliseconds. If this value
                                    is less than zero, it will wait forever.
        @returns    true if the thread exits, or false if the timeout expires first.
    */
    bool wait(unsigned long timeOutMilliseconds = ULONG_MAX);

    /** Returns an id that identifies the caller thread.

        To find the ID of a particular thread object, use getThreadId().

        @returns    a unique identifier that identifies the calling thread.
        @see getThreadId
    */
    static ThreadID currentThreadId();

    /** Finds the thread object that is currently running.

        Note that the main UI thread (or other non-JUCE threads) don't have a Thread
        object associated with them, so this will return nullptr.
    */
    static Thread* currentThread();

    /** Suspends the execution of the current thread until the specified timeout period
        has elapsed (note that this may not be exact).

        The timeout period must not be negative and whilst sleeping the thread cannot
        be woken up so it should only be used for short periods of time and when other
        methods such as using a WaitableEvent or CriticalSection are not possible.
    */
    static void sleep(int milliseconds);

    /** Yields the current thread's CPU time-slot and allows a new thread to run.

        If there are no other threads of equal or higher priority currently running then
        this will return immediately and the current thread will continue to run.
    */
    static void yield();

    virtual void run() = 0;

private:
    std::string makeName();
    /// Creates a unique name for a thread.

    static unsigned int uniqueId();
    /// Creates and returns a unique id for a thread.

    void threadCleanup();

private:
    struct ThreadData;
    std::unique_ptr<ThreadData> d_ptr;

    static thread_local Thread* m_thread;
    static thread_local Thread* m_threadId;

    friend class PooledThread;

    GL_DECLARE_NON_COPYABLE(Thread)
};

} // namespace glue
