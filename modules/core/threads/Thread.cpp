//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "threads/Thread.h"

#include "logging/Log.h"

// #if GLUE_ANDROID
//     #include "ofxAndroidUtils.h"
//     #include <jni.h>
// #endif

namespace glue
{

Thread::Thread() : threadRunning(false), threadDone(true), mutexBlocks(true), name("") {}

//-------------------------------------------------
bool Thread::isThreadRunning() const { return threadRunning; }

//-------------------------------------------------
std::thread::id Thread::getThreadId() const { return thread.get_id(); }

//-------------------------------------------------
std::string Thread::getThreadName() const { return name; }

//-------------------------------------------------
void Thread::setThreadName(const std::string& name) { this->name = name; }

//-------------------------------------------------
void Thread::startThread()
{
    std::unique_lock<std::mutex> lck(mutex);
    if (threadRunning || !threadDone)
    {
        LogWarn("Thread") << "- name: " << getThreadName()
                          << " - Cannot start, thread already running.";
        return;
    }

    threadDone = false;
    threadRunning = true;
    this->mutexBlocks = true;

    thread = std::thread(std::bind(&Thread::threadEntry, this));
}

//-------------------------------------------------
bool Thread::lock()
{
    if (mutexBlocks) { mutex.lock(); }
    else
    {
        if (!mutex.try_lock())
        {
            return false; // mutex is locked, try_lock failed
        }
    }
    return true;
}

//-------------------------------------------------
bool Thread::tryLock() { return mutex.try_lock(); }

//-------------------------------------------------
void Thread::unlock() { mutex.unlock(); }

//-------------------------------------------------
void Thread::stopThread() { threadRunning = false; }

//-------------------------------------------------
void Thread::waitForThread(bool callStopThread, long milliseconds)
{
    if (!threadDone)
    {
        // tell thread to stop
        if (callStopThread) { stopThread(); }

        // wait for the thread to finish
        if (isCurrentThread())
        {
            return; // waitForThread should only be called outside thread
        }

        if (INFINITE_JOIN_TIMEOUT == milliseconds)
        {
            std::unique_lock<std::mutex> lck(mutex);
            if (!threadDone) { condition.wait(lck); }
        }
        else
        {
            // Wait for "joinWaitMillis" milliseconds for thread to finish
            std::unique_lock<std::mutex> lck(mutex);
            if (!threadDone && condition.wait_for(lck, std::chrono::milliseconds(milliseconds)) ==
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
bool Thread::isCurrentThread() const { return std::this_thread::get_id() == thread.get_id(); }

//-------------------------------------------------
std::thread& Thread::getNativeThread() { return thread; }

//-------------------------------------------------
const std::thread& Thread::getNativeThread() const { return thread; }

//-------------------------------------------------
void Thread::run()
{
    LogWarn() << "- name: " << getThreadName()
              << " - Override Thread::run() in your Thread subclass.";
}

//-------------------------------------------------
void Thread::threadEntry()
{
    // #if GLUE_ANDROID
    //     JNIEnv* env;
    //     jint attachResult = ofGetJavaVMPtr()->AttachCurrentThread(&env, nullptr);
    //     if (attachResult != 0) { LogWarn() << "couldn't attach new thread to java vm"; }
    // #endif

    // user function
    // should loop endlessly.
    try
    {
        run();
    } catch (const std::exception& exc)
    {
        LogFatal() << exc.what();
    } catch (...)
    {
        LogFatal() << "Unknown exception.";
    }
    try
    {
        thread.detach();
    } catch (...)
    {
    }
    // #if GLUE_ANDROID
    //     attachResult = ofGetJavaVMPtr()->DetachCurrentThread();
    // #endif

    std::unique_lock<std::mutex> lck(mutex);
    threadRunning = false;
    threadDone = true;
    condition.notify_all();
}

} // namespace glue
