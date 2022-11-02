//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/StandardHeader.h"

namespace glue
{

/// The Runnable interface with the run() method
/// must be implemented by classes that provide
/// an entry point for a thread.
class GLUE_API Runnable
{
public:
    virtual ~Runnable() = default;

    /// Do whatever the thread needs to do. Must
    /// be overridden by subclasses.
    virtual void run() = 0;
};

/// \class Thread
/// \brief A threaded base class with a built in mutex for convenience.
///
class GLUE_API Thread final
{
public:
    /// \brief Create an Thread.
    Thread();

    /// \brief Create an Thread.
    Thread(const std::string& name);

    /// \brief Check the running status of the thread.
    /// \returns true iff the thread is currently running.
    bool isRunning() const;

    /// \brief Get the unique thread id.
    /// \note This is NOT the the same as the operating thread id!
    std::thread::id getThreadId() const;

    /// \brief Get the unique thread name, in the form of "Thread id#"
    /// \returns the Thread ID string.
    std::string getName() const;

    void setName(const std::string& name);

    /// \brief Starts the thread with the given target.
    ///
    /// Note that the given Runnable object must remain
    /// valid during the entire lifetime of the thread, as
    /// only a reference to it is stored internally.
    void start(Runnable& target);

    /// \brief Starts the thread with the given target.
    ///
    /// The Thread ensures that the given target stays
    /// alive while the thread is running.
    void start(std::shared_ptr<Runnable> pTarget);

    /// \brief Stop the thread.
    ///
    /// This does immediately stop the thread from processing, but
    /// will only set a flag that must be checked from within your
    /// threadedFunction() by calling isRunning().  If the user wants
    /// to both stop the thread AND wait for the thread to finish
    /// processing, the user should call waitForThread(true, ...).
    void stop();

    /// \brief Wait for the thread to exit (aka "joining" the thread).
    ///
    /// This method waits for a thread will "block" and wait for the
    /// thread (aka "join" the thread) before it returns.  This allows the
    /// user to be sure that the thread is properly cleaned up.  An example
    /// of when this might be particularly important is if the
    /// threadedFunction() is opening a set of network sockets, or
    /// downloading data from the web.  Destroying an Thread subclass
    /// without releasing those sockets (or other resources), may result in
    /// segmentation faults, error signals or other undefined behaviors.
    ///
    /// \param callStopThread Set stop to true if you want to signal the thread
    ///     to exit before waiting.  This is the equivalent to calling
    ///     stop(). If you your threadedFunction uses a while-loop that
    ///     depends on isRunning() and you do not call stop() or set
    ///     stop == true, waitForThread will hang indefinitely.  Set stop ==
    ///     false ONLY if you have already called stop() and you simply
    ///     need to be sure your thread has finished its tasks.
    ///
    /// \param milliseconds If millseconds is set to INFINITE_JOIN_TIMEOUT, the
    ///     waitForThread will wait indefinitely for the thread to complete.  If
    ///     milliseconds is set to a lower number (e.g. 10000 for 10 seconds),
    ///     waitForThread will wait for 10000 milliseconds and if the thread has
    ///     not yet stopped it will return and log an error message.  Users are
    ///     encouraged to use the default INFINITE_JOIN_TIMEOUT.  If the user is
    ///     unhappy with the amount of time it takes to join a thread, the user
    ///     is encouraged to seek more expedient ways of signalling their desire
    ///     for a thread to complete via other signalling methods such as
    ///     GLUE::Event, GLUE::Condition, or GLUE::Semaphore.
    void waitForThread(bool callStopThread = true, long milliseconds = INFINITE_JOIN_TIMEOUT);

    /// \brief Tell the thread to sleep for a certain amount of milliseconds.
    ///
    /// This is useful inside the threadedFunction() when a thread is waiting
    /// for input to process:
    ///
    ///     void MyThreadedClass::threadedFunction()
    ///     {
    ///		    // start
    ///		    while(isRunning())
    ///         {
    ///             // bReadyToProcess can be set from outside the threadedFuntion.
    ///             // perhaps by another thread that downloads data, or loads
    ///             // some media, etc.
    ///
    ///		    	if(bReadyToProcess == true)
    ///             {
    ///		    		// do some time intensive processing
    ///		    		bReadyToProcess = false;
    ///		    	}
    ///             else
    ///             {
    ///		    		// sleep the thread to give up some cpu
    ///		    		sleep(20);
    ///		    	}
    ///		    }
    ///		    // done
    ///     }
    ///
    /// If the user does not give the thread a chance to sleep, the
    /// thread may take 100% of the CPU core while it's looping as it
    /// waits for something to do.  This may lead to poor application
    /// performance.
    ///
    /// \param milliseconds The number of milliseconds to sleep.
    static void sleep(long milliseconds);

    /// \brief Tell the thread to give up its CPU time other threads.
    ///
    /// This method is similar to sleep() and can often be used in
    /// the same way.  The main difference is that 1 millisecond
    /// (the minimum sleep time available with sleep()) is a very
    /// long time on modern processors and yield() simply gives up
    /// processing time to the next thread, instead of waiting for
    /// number of milliseconds. In some cases, this behavior will
    /// be preferred.
    static void yield();

    /// \brief Query whether the current thread is active.
    ///
    /// In multithreaded situations, it can be useful to know which
    /// thread is currently running some code in order to make sure
    /// only certain threads can do certain things.  For example,
    /// OpenGL can only run in the main execution thread.  Thus,
    /// situations where a thread is responsible for interacting
    /// with graphics resources may need to prevent graphics updates
    /// unless the main thread is accessing or updating resources
    /// shared with this Thread (or its subclass).
    ///
    ///     if(myThread.isCurrentThread())
    ///     {
    ///         // do some myThread things,
    ///         // but keep your hands off my resources!
    ///     }
    ///     else if(Thread::isMainThread())
    ///     {
    ///         // pheew! ok, update those graphics resources
    ///     }
    ///
    /// By way of another example, a subclass of Thread may have
    /// an update() method that is called from ofBaseApp during the
    /// execution of the main application thread.  In these cases,
    /// the Thread subclass might want to ask itself whether it
    /// can, for instance, call update() on an ofImage, in order to
    /// send copy some ofPixels to an ofTexture on the graphics
    /// card.
    ///
    /// \returns True iff this Thread the currently active thread.
    bool isCurrentThread() const;

    /// \brief Get a reference to the underlying GLUE thread.
    ///
    /// GLUE::Thread provides a clean cross-platform wrapper for
    /// threads.  On occasion, it may be useful to interact with the
    /// underlying GLUE::Thread directly.
    ///
    /// \returns A reference to the backing GLUE thread.
    std::thread& getNativeThread();

    /// \brief Get a const reference to the underlying GLUE thread.
    ///
    /// GLUE::Thread provides a clean cross-platform wrapper for
    /// threads.  On occasion, it may be useful to interact with the
    /// underlying GLUE::Thread directly.
    ///
    /// \returns A reference to the backing GLUE thread.
    const std::thread& getNativeThread() const;

    enum
    {
        INFINITE_JOIN_TIMEOUT = -1
        ///< \brief A sentinal value for an infinite join timeout.
        ///<
        ///< Primarily used with the waitForThread() method.
    };

protected:
    std::thread m_thread;
    mutable std::mutex m_mutex;

private:
    void threadEntry();

    ///< \brief Is the thread running?
    std::atomic<bool> m_threadRunning;
    std::atomic<bool> m_threadDone;

    std::string m_name;
    std::condition_variable m_condition;

    std::shared_ptr<Runnable> m_pRunnableTarget;
};

} // namespace glue
