//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

//==============================================================================
#ifndef DOXYGEN
    #define GLUE_JOIN_MACRO_HELPER(a, b) a##b
    #define GLUE_STRINGIFY_MACRO_HELPER(a) #a
#endif

/** A good old-fashioned C macro concatenation helper.
    This combines two items (which may themselves be macros) into a single string,
    avoiding the pitfalls of the ## macro operator.
*/
#define GLUE_JOIN_MACRO(item1, item2) GLUE_JOIN_MACRO_HELPER(item1, item2)

/** A handy C macro for stringifying any symbol, rather than just a macro parameter. */
#define GLUE_STRINGIFY(item) GLUE_STRINGIFY_MACRO_HELPER(item)

#define GLUE_CHECK_ALIGN(ptr, alignment)                                                           \
    do {                                                                                           \
        constexpr size_t status = reinterpret_cast<uintptr_t>(ptr) % alignment;                    \
        static_assert(status == 0, "ptr must be aligned");                                         \
    } while (0)

#define GLUE_DECLARE_NON_COPYABLE(className)                                                       \
    className(const className&) = delete;                                                          \
    className& operator=(const className&) = delete;

/** This is a shorthand way of writing both a GLUE_DECLARE_NON_COPYABLE and
    GLUE_LEAK_DETECTOR macro for a class.
*/
#define GLUE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(className)                                    \
    GLUE_DECLARE_NON_COPYABLE(className)                                                           \
    GLUE_LEAK_DETECTOR(className)

/** This macro can be added to class definitions to disable the use of new/delete to
    allocate the object on the heap, forcing it to only be used as a stack or member variable.
*/
#define GLUE_PREVENT_HEAP_ALLOCATION                                                               \
private:                                                                                           \
    static void* operator new(size_t) = delete;                                                    \
    static void operator delete(void*) = delete;

//==============================================================================
// Debugging and assertion macros

#ifndef GLUE_LOG_CURRENT_ASSERTION
    #if GLUE_LOG_ASSERTIONS || GLUE_DEBUG
        #define GLUE_LOG_CURRENT_ASSERTION glue::logAssertion(__FILE__, __LINE__);
    #else
        #define GLUE_LOG_CURRENT_ASSERTION
    #endif
#endif

//==============================================================================
/** This will try to break into the debugger if the app is currently being debugged.
    If called by an app that's not being debugged, the behaviour isn't defined - it may
    crash or not, depending on the platform.
    @see GLUE_ASSERT()
*/
#if defined(GLUE_OS_LINUX) || defined(GLUE_OS_BSD)
    #define GLUE_BREAK_IN_DEBUGGER ::kill(0, SIGTRAP);
#elif defined(GLUE_COMPILER_MSVC)
    #ifndef __INTEL_COMPILER
        #pragma intrinsic(__debugbreak)
    #endif
    #define GLUE_BREAK_IN_DEBUGGER __debugbreak();
#elif GLUE_ARCH_X86 && defined(GLUE_USE_GCC_INLINE_ASM)
    #define GLUE_BREAK_IN_DEBUGGER asm("int $3");
#elif defined(GLUE_OS_ANDROID)
    #define GLUE_BREAK_IN_DEBUGGER __builtin_trap();
#else
    #define GLUE_BREAK_IN_DEBUGGER __asm int 3
#endif

//==============================================================================
#if GLUE_MSVC && !defined(DOXYGEN)
    #define GLUE_WHILE_LOOP(x)                                                                     \
        __pragma(warning(push)) __pragma(warning(disable : 4127)) do                               \
        {                                                                                          \
            x                                                                                      \
        }                                                                                          \
        while (false) __pragma(warning(pop))
#else
    /** This is the good old C++ trick for creating a macro that forces the user to put
       a semicolon after it when they use it.
    */
    #define GLUE_WHILE_LOOP(x)                                                                     \
        do {                                                                                       \
            x                                                                                      \
        } while (false)
#endif

//==============================================================================
/** This will always cause an assertion failure.
    It is only compiled in a debug build, (unless GLUE_LOG_ASSERTIONS is enabled for your
   build).
    @see jassert
*/
#define jassertfalse GLUE_WHILE_LOOP(GLUE_LOG_CURRENT_ASSERTION)

/** Platform-independent assertion macro.

    This macro gets turned into a no-op when you're building with debugging turned off, so be
    careful that the expression you pass to it doesn't perform any actions that are vital for
   the correct behaviour of your program!
    @see jassertfalse
*/
#define jassert(expression) GLUE_WHILE_LOOP(if (!(expression)) jassertfalse;)

/** Platform-independent assertion macro which suppresses ignored-variable
    warnings in all build modes. You should probably use a plain jassert()
    by default, and only replace it with jassertquiet() once you've
    convinced yourself that any unused-variable warnings emitted by the
    compiler are harmless.
*/
#define jassertquiet(expression) GLUE_WHILE_LOOP(if (!(expression)) jassertfalse;)
