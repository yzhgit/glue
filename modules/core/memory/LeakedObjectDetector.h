//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "StandardHeader.h"

#include "memory/Atomic.h"

namespace glue
{

//==============================================================================
/**
    Embedding an instance of this class inside another class can be used as a low-overhead
    way of detecting leaked instances.

    This class keeps an internal static count of the number of instances that are
    active, so that when the app is shutdown and the static destructors are called,
    it can check whether there are any left-over instances that may have been leaked.

    To use it, use the GLUE_LEAK_DETECTOR macro as a simple way to put one in your
    class declaration. Have a look through the glue codebase for examples, it's used
    in most of the classes.

    @tags{Core}
*/
template <class OwnerClass>
class LeakedObjectDetector
{
public:
    //==============================================================================
    LeakedObjectDetector() noexcept { ++(getCounter().numObjects); }
    LeakedObjectDetector(const LeakedObjectDetector&) noexcept { ++(getCounter().numObjects); }

    LeakedObjectDetector& operator=(const LeakedObjectDetector&) noexcept = default;

    ~LeakedObjectDetector()
    {
        if (--(getCounter().numObjects) < 0)
        {
            DBG("*** Dangling pointer deletion! Class: " << getLeakedObjectClassName());

            /** If you hit this, then you've managed to delete more instances of this class than
               you've created.. That indicates that you're deleting some dangling pointers.

                Note that although this assertion will have been triggered during a destructor, it
               might not be this particular deletion that's at fault - the incorrect one may have
               happened at an earlier point in the program, and simply not been detected until now.

                Most errors like this are caused by using old-fashioned, non-RAII techniques for
                your object management. Tut, tut. Always, always use std::unique_ptrs, OwnedArrays,
                ReferenceCountedObjects, etc, and avoid the 'delete' operator at all costs!
            */
            jassertfalse;
        }
    }

private:
    //==============================================================================
    class LeakCounter
    {
    public:
        LeakCounter() = default;

        ~LeakCounter()
        {
            if (numObjects.value > 0)
            {
                DBG("*** Leaked objects detected: " << numObjects.value << " instance(s) of class "
                                                    << getLeakedObjectClassName());

                /** If you hit this, then you've leaked one or more objects of the type specified by
                    the 'OwnerClass' template parameter - the name should have been printed by the
                   line above.

                    If you're leaking, it's probably because you're using old-fashioned, non-RAII
                   techniques for your object management. Tut, tut. Always, always use
                   std::unique_ptrs, OwnedArrays, ReferenceCountedObjects, etc, and avoid the
                   'delete' operator at all costs!
                */
                jassertfalse;
            }
        }

        Atomic<int> numObjects;
    };

    static const char* getLeakedObjectClassName() { return OwnerClass::getLeakedObjectClassName(); }

    static LeakCounter& getCounter() noexcept
    {
        static LeakCounter counter;
        return counter;
    }
};

//==============================================================================
#if DOXYGEN || !defined(GLUE_LEAK_DETECTOR)
    #if (DOXYGEN || GLUE_CHECK_MEMORY_LEAKS)
        /** This macro lets you embed a leak-detecting object inside a class.

            To use it, simply declare a GLUE_LEAK_DETECTOR(YourClassName) inside a private section
            of the class declaration. E.g.

            @code
            class MyClass
            {
            public:
                MyClass();
                void blahBlah();

            private:
                GLUE_LEAK_DETECTOR (MyClass)
            };
            @endcode

            @see GLUE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR, LeakedObjectDetector
        */
        #define GLUE_LEAK_DETECTOR(OwnerClass)                                                     \
            friend class glue::LeakedObjectDetector<OwnerClass>;                                   \
            static const char* getLeakedObjectClassName() noexcept { return #OwnerClass; }         \
            glue::LeakedObjectDetector<OwnerClass> GLUE_JOIN_MACRO(leakDetector, __LINE__);
    #else
        #define GLUE_LEAK_DETECTOR(OwnerClass)
    #endif
#endif

} // namespace glue
