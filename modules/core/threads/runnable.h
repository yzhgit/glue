//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/precompiled.h"

namespace glue
{

/// The Runnable interface with the run() method
/// must be implemented by classes that provide
/// an entry point for a thread.
class GLUE_API Runnable
{
public:
    Runnable();
    virtual ~Runnable();

    /// Do whatever the thread needs to do. Must
    /// be overridden by subclasses.
    virtual void run() = 0;
};

/// This adapter simplifies using ordinary methods as
/// targets for threads.
/// Usage:
///    RunnableAdapter<MyClass> ra(myObject, &MyObject::doSomething));
///    Thread thr;
///    thr.Start(ra);
///
/// For using a freestanding or static member function as a thread
/// target, please see the ThreadTarget class.
template <class C>
class RunnableAdapter : public Runnable
{
public:
    typedef void (C::*Callback)();

    RunnableAdapter(C& object, Callback method) : m_pObject(&object), m_method(method) {}

    RunnableAdapter(const RunnableAdapter& ra) : m_pObject(ra.m_pObject), m_method(ra.m_method) {}

    ~RunnableAdapter() {}

    RunnableAdapter& operator=(const RunnableAdapter& ra)
    {
        m_pObject = ra.m_pObject;
        m_method = ra.m_method;
        return *this;
    }

    void run() { (m_pObject->*m_method)(); }

private:
    RunnableAdapter();

    C* m_pObject;
    Callback m_method;
};

template <class Functor>
class FunctorRunnable : public Runnable
{
public:
    FunctorRunnable(const Functor& functor) : _functor(functor) {}

    FunctorRunnable(Functor&& functor) : _functor(std::move(functor)) {}

    ~FunctorRunnable() {}

    void run() { _functor(); }

private:
    Functor _functor;
};

} // namespace glue
