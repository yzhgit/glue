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

// GLUE_ASSERT()
//
// In C++11, `assert` can't be used portably within constexpr functions.
// GLUE_ASSERT functions as a runtime assert but works in C++11 constexpr
// functions.  Example:
//
// constexpr double Divide(double a, double b) {
//   return GLUE_ASSERT(b != 0), a / b;
// }
//
// This macro is inspired by
// https://akrzemi1.wordpress.com/2017/05/18/asserts-in-constexpr-functions/
#if defined(NDEBUG)
    #define GLUE_ASSERT(expr) (false ? static_cast<void>(expr) : static_cast<void>(0))
#else
    #include <cassert>
    #define GLUE_ASSERT(expr)                                                                      \
        (GLUE_LIKELY((expr)) ? static_cast<void>(0) : [] { assert(false && #expr); }())
#endif

#define GLUE_ASSERT_MSG(expr, msg) GLUE_ASSERT(expr)

// Utility function to selectively silence unused variable compiler warnings
template <typename... Ts>
inline void IgnoreUnused(Ts&&...)
{}
