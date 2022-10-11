//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#define CHECK_ALIGN(ptr, alignment)                                                                \
    do {                                                                                           \
        constexpr size_t status = reinterpret_cast<uintptr_t>(ptr) % alignment;                    \
        static_assert(status == 0, "ptr must be aligned");                                         \
    } while (0)

/* Offset of member MEMBER in a struct of type TYPE. */
#define offsetof(OBJECT, MEMBER) __builtin_offsetof(OBJECT, MEMBER)

#if __cplusplus > 201103L
    #define GL_DEPRECATED(reason) [[deprecated(reason)]]
#elif defined(__clang__)
    #define GL_DEPRECATED(reason) __attribute__((deprecated(reason)))
#elif defined(__GNUG__)
    #define GL_DEPRECATED(reason) __attribute__((deprecated))
#elif defined(_MSC_VER)
    #if _MSC_VER < 1910
        #define GL_DEPRECATED(reason) __declspec(deprecated)
    #else
        #define GL_DEPRECATED(reason) [[deprecated(reason)]]
    #endif
#endif

#define GL_DECLARE_NON_COPYABLE(className)                                                         \
    className(const className&) = delete;                                                          \
    className& operator=(const className&) = delete;

/** This is a shorthand macro for deleting a class's move constructor and
    move assignment operator.
*/
#define GL_DECLARE_NON_MOVEABLE(className)                                                         \
    className(className&&) = delete;                                                               \
    className& operator=(className&&) = delete;
