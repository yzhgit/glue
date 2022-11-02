//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/TargetPlatform.h"

//==============================================================================
/*  This file defines miscellaneous macros for debugging, assertions, etc.
 */

//==============================================================================

/** Push/pop warnings on MSVC. These macros expand to nothing on other
    compilers (like clang and gcc).
*/
#if GLUE_MSVC
    #define GLUE_IGNORE_MSVC(warnings) __pragma(warning(disable : warnings))
    #define GLUE_BEGIN_IGNORE_WARNINGS_LEVEL_MSVC(level, warnings)                                 \
        __pragma(warning(push, level)) GLUE_IGNORE_MSVC(warnings)
    #define GLUE_BEGIN_IGNORE_WARNINGS_MSVC(warnings)                                              \
        __pragma(warning(push)) GLUE_IGNORE_MSVC(warnings)
    #define GLUE_END_IGNORE_WARNINGS_MSVC __pragma(warning(pop))
#else
    #define GLUE_IGNORE_MSVC(warnings)
    #define GLUE_BEGIN_IGNORE_WARNINGS_LEVEL_MSVC(level, warnings)
    #define GLUE_BEGIN_IGNORE_WARNINGS_MSVC(warnings)
    #define GLUE_END_IGNORE_WARNINGS_MSVC
#endif

/** This macro defines the C calling convention used as the standard for GLUE_ calls. */
#if GLUE_WINDOWS
    #define GLUE_CALLTYPE __stdcall
    #define GLUE_CDECL __cdecl
#else
    #define GLUE_CALLTYPE
    #define GLUE_CDECL
#endif

//==============================================================================
#if GLUE_LINUX || GLUE_BSD
    /** This will try to break into the debugger if the app is currently being debugged.
        If called by an app that's not being debugged, the behaviour isn't defined - it may
        crash or not, depending on the platform.
        @see GLUE_ASSERT()
    */
    #define GLUE_BREAK_IN_DEBUGGER                                                                 \
        {                                                                                          \
            ::kill(0, SIGTRAP);                                                                    \
        }
#elif GLUE_MSVC
    #ifndef __INTEL_COMPILER
        #pragma intrinsic(__debugbreak)
    #endif
    #define GLUE_BREAK_IN_DEBUGGER                                                                 \
        {                                                                                          \
            __debugbreak();                                                                        \
        }
#elif GLUE_INTEL && (GLUE_GCC || GLUE_CLANG)
    #if GLUE_NO_INLINE_ASM
        #define GLUE_BREAK_IN_DEBUGGER                                                             \
            {                                                                                      \
            }
    #else
        #define GLUE_BREAK_IN_DEBUGGER                                                             \
            {                                                                                      \
                asm("int $3");                                                                     \
            }
    #endif
#elif GLUE_ANDROID
    #define GLUE_BREAK_IN_DEBUGGER                                                                 \
        {                                                                                          \
            __builtin_trap();                                                                      \
        }
#else
    #define GLUE_BREAK_IN_DEBUGGER                                                                 \
        {                                                                                          \
            __asm int 3                                                                            \
        }
#endif

#if GLUE_CLANG && defined(__has_feature) && !defined(GLUE_ANALYZER_NORETURN)
    #if __has_feature(attribute_analyzer_noreturn)
inline void __attribute__((analyzer_noreturn)) glue_assert_noreturn() {}
        #define GLUE_ANALYZER_NORETURN glue::glue_assert_noreturn();
    #endif
#endif

#ifndef GLUE_ANALYZER_NORETURN
    #define GLUE_ANALYZER_NORETURN
#endif

/** Used to silence Wimplicit-fallthrough on Clang and GCC where available
    as there are a few places in the codebase where we need to do this
    deliberately and want to ignore the warning.
*/
#if GLUE_CLANG
    #if __has_cpp_attribute(clang::fallthrough)
        #define GLUE_FALLTHROUGH [[clang::fallthrough]];
    #else
        #define GLUE_FALLTHROUGH
    #endif
#elif GLUE_GCC
    #if __GNUC__ >= 7
        #define GLUE_FALLTHROUGH [[gnu::fallthrough]];
    #else
        #define GLUE_FALLTHROUGH
    #endif
#else
    #define GLUE_FALLTHROUGH
#endif

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

/* Offset of member MEMBER in a struct of type TYPE. */
#define offsetof(OBJECT, MEMBER) __builtin_offsetof(OBJECT, MEMBER)

#define GLUE_DECLARE_NON_COPYABLE(className)                                                       \
    className(const className&) = delete;                                                          \
    className& operator=(const className&) = delete;

/** This is a shorthand macro for deleting a class's move constructor and
    move assignment operator.
*/
#define GLUE_DECLARE_NON_MOVEABLE(className)                                                       \
    className(className&&) = delete;                                                               \
    className& operator=(className&&) = delete;

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

// -----------------------------------------------------------------------------
// Compiler Feature Checks
// -----------------------------------------------------------------------------

// GLUE_HAVE_BUILTIN()
//
// Checks whether the compiler supports a Clang Feature Checking Macro, and if
// so, checks whether it supports the provided builtin function "x" where x
// is one of the functions noted in
// https://clang.llvm.org/docs/LanguageExtensions.html
//
// Note: Use this macro to avoid an extra level of #ifdef __has_builtin check.
// http://releases.llvm.org/3.3/tools/clang/docs/LanguageExtensions.html
#ifdef __has_builtin
    #define GLUE_HAVE_BUILTIN(x) __has_builtin(x)
#else
    #define GLUE_HAVE_BUILTIN(x) 0
#endif

#ifdef __has_feature
    #define GLUE_HAVE_FEATURE(f) __has_feature(f)
#else
    #define GLUE_HAVE_FEATURE(f) 0
#endif

// GLUE_HAVE_ATTRIBUTE
//
// A function-like feature checking macro that is a wrapper around
// `__has_attribute`, which is defined by GCC 5+ and Clang and evaluates to a
// nonzero constant integer if the attribute is supported or 0 if not.
//
// It evaluates to zero if `__has_attribute` is not defined by the compiler.
//
// GCC: https://gcc.gnu.org/gcc-5/changes.html
// Clang: https://clang.llvm.org/docs/LanguageExtensions.html
#ifdef __has_attribute
    #define GLUE_HAVE_ATTRIBUTE(x) __has_attribute(x)
#else
    #define GLUE_HAVE_ATTRIBUTE(x) 0
#endif

// GLUE_HAVE_CPP_ATTRIBUTE
//
// A function-like feature checking macro that accepts C++11 style attributes.
// It's a wrapper around `__has_cpp_attribute`, defined by ISO C++ SD-6
// (https://en.cppreference.com/w/cpp/experimental/feature_test). If we don't
// find `__has_cpp_attribute`, will evaluate to 0.
#if defined(__cplusplus) && defined(__has_cpp_attribute)
    // NOTE: requiring __cplusplus above should not be necessary, but
    // works around https://bugs.llvm.org/show_bug.cgi?id=23435.
    #define GLUE_HAVE_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
    #define GLUE_HAVE_CPP_ATTRIBUTE(x) 0
#endif

// -----------------------------------------------------------------------------
// Function Attributes
// -----------------------------------------------------------------------------
//
// GCC: https://gcc.gnu.org/onlinedocs/gcc/Function-Attributes.html
// Clang: https://clang.llvm.org/docs/AttributeReference.html

// GLUE_CHECK_FMT
//
// Tells the compiler to perform `printf` format string checking if the
// compiler supports it; see the 'format' attribute in
// <https://gcc.gnu.org/onlinedocs/gcc-4.7.0/gcc/Function-Attributes.html>.
//
// Note: As the GCC manual states, "[s]ince non-static C++ methods
// have an implicit 'this' argument, the arguments of such methods
// should be counted from two, not one."
#if GLUE_HAVE_ATTRIBUTE(format) || (defined(__GNUC__) && !defined(__clang__))
    #define GLUE_CHECK_FMT(string_index, first_to_check)                                           \
        __attribute__((__format__(__printf__, string_index, first_to_check)))
#else
    #define GLUE_CHECK_FMT(string_index, first_to_check)
#endif

// GLUE_ALWAYS_INLINE
//
// Forces functions to either inline or not inline. Introduced in gcc 3.1.
#if GLUE_HAVE_ATTRIBUTE(always_inline) || (defined(__GNUC__) && !defined(__clang__))
    #define GLUE_ALWAYS_INLINE __attribute__((always_inline))
    #define GL_HAVE_ATTRIBUTE_ALWAYS_INLINE 1
#else
    #define GLUE_ALWAYS_INLINE
#endif

// GLUE_WEAK
//
// Tags a function as weak for the purposes of compilation and linking.
// Weak attributes did not work properly in LLVM's Windows backend before
// 9.0.0, so disable them there. See https://bugs.llvm.org/show_bug.cgi?id=37598
// for further information.
// The MinGW compiler doesn't complain about the weak attribute until the link
// step, presumably because Windows doesn't use ELF binaries.
#if (GLUE_HAVE_ATTRIBUTE(weak) || (defined(__GNUC__) && !defined(__clang__))) &&                   \
    (!defined(_WIN32) || (defined(__clang__) && __clang_major__ >= 9)) && !defined(__MINGW32__)
    #undef GLUE_WEAK
    #define GLUE_WEAK __attribute__((weak))
    #define GLUE_HAVE_ATTRIBUTE_WEAK 1
#else
    #define GLUE_WEAK
    #define GLUE_HAVE_ATTRIBUTE_WEAK 0
#endif

// GLUE_NONNULL
//
// Tells the compiler either (a) that a particular function parameter
// should be a non-null pointer, or (b) that all pointer arguments should
// be non-null.
//
// Note: As the GCC manual states, "[s]ince non-static C++ methods
// have an implicit 'this' argument, the arguments of such methods
// should be counted from two, not one."
//
// Args are indexed starting at 1.
//
// For non-static class member functions, the implicit `this` argument
// is arg 1, and the first explicit argument is arg 2. For static class member
// functions, there is no implicit `this`, and the first explicit argument is
// arg 1.
//
// Example:
//
//   /* arg_a cannot be null, but arg_b can */
//   void Function(void* arg_a, void* arg_b) GLUE_NONNULL(1);
//
//   class C {
//     /* arg_a cannot be null, but arg_b can */
//     void Method(void* arg_a, void* arg_b) GLUE_NONNULL(2);
//
//     /* arg_a cannot be null, but arg_b can */
//     static void StaticMethod(void* arg_a, void* arg_b)
//     GLUE_NONNULL(1);
//   };
//
// If no arguments are provided, then all pointer arguments should be non-null.
//
//  /* No pointer arguments may be null. */
//  void Function(void* arg_a, void* arg_b, int arg_c) GLUE_NONNULL();
//
// NOTE: The GCC nonnull attribute actually accepts a list of arguments, but
// GLUE_NONNULL does not.
#if GLUE_HAVE_ATTRIBUTE(nonnull) || (defined(__GNUC__) && !defined(__clang__))
    #define GLUE_NONNULL(arg_index) __attribute__((nonnull(arg_index)))
#else
    #define GLUE_NONNULL(...)
#endif

// GLUE_NO_SANITIZE_ADDRESS
//
// Tells the AddressSanitizer (or other memory testing tools) to ignore a given
// function. Useful for cases when a function reads random locations on stack,
// calls _exit from a cloned subprocess, deliberately accesses buffer
// out of bounds or does other scary things with memory.
// NOTE: GCC supports AddressSanitizer(asan) since 4.8.
// https://gcc.gnu.org/gcc-4.8/changes.html
#if GLUE_HAVE_ATTRIBUTE(no_sanitize_address)
    #define GLUE_NO_SANITIZE_ADDRESS __attribute__((no_sanitize_address))
#elif defined(_MSC_VER) && _MSC_VER >= 1928
    // https://docs.microsoft.com/en-us/cpp/cpp/no-sanitize-address
    #define GLUE_NO_SANITIZE_ADDRESS __declspec(no_sanitize_address)
#else
    #define GLUE_NO_SANITIZE_ADDRESS
#endif

// GLUE_NO_SANITIZE_MEMORY
//
// Tells the MemorySanitizer to relax the handling of a given function. All "Use
// of uninitialized value" warnings from such functions will be suppressed, and
// all values loaded from memory will be considered fully initialized.  This
// attribute is similar to the GLUE_NO_SANITIZE_ADDRESS attribute
// above, but deals with initialized-ness rather than addressability issues.
// NOTE: MemorySanitizer(msan) is supported by Clang but not GCC.
#if GLUE_HAVE_ATTRIBUTE(no_sanitize_memory)
    #define GLUE_NO_SANITIZE_MEMORY __attribute__((no_sanitize_memory))
#else
    #define GLUE_NO_SANITIZE_MEMORY
#endif

// GLUE_NO_SANITIZE_THREAD
//
// Tells the ThreadSanitizer to not instrument a given function.
// NOTE: GCC supports ThreadSanitizer(tsan) since 4.8.
// https://gcc.gnu.org/gcc-4.8/changes.html
#if GLUE_HAVE_ATTRIBUTE(no_sanitize_thread)
    #define GLUE_NO_SANITIZE_THREAD __attribute__((no_sanitize_thread))
#else
    #define GLUE_NO_SANITIZE_THREAD
#endif

// GLUE_NO_SANITIZE_UNDEFINED
//
// Tells the UndefinedSanitizer to ignore a given function. Useful for cases
// where certain behavior (eg. division by zero) is being used intentionally.
// NOTE: GCC supports UndefinedBehaviorSanitizer(ubsan) since 4.9.
// https://gcc.gnu.org/gcc-4.9/changes.html
#if GLUE_HAVE_ATTRIBUTE(no_sanitize_undefined)
    #define GLUE_NO_SANITIZE_UNDEFINED __attribute__((no_sanitize_undefined))
#elif GLUE_HAVE_ATTRIBUTE(no_sanitize)
    #define GLUE_NO_SANITIZE_UNDEFINED __attribute__((no_sanitize("undefined")))
#else
    #define GLUE_NO_SANITIZE_UNDEFINED
#endif

// GLUE_NO_SANITIZE_CFI
//
// Tells the ControlFlowIntegrity sanitizer to not instrument a given function.
// See https://clang.llvm.org/docs/ControlFlowIntegrity.html for details.
#if GLUE_HAVE_ATTRIBUTE(no_sanitize)
    #define GLUE_NO_SANITIZE_CFI __attribute__((no_sanitize("cfi")))
#else
    #define GLUE_NO_SANITIZE_CFI
#endif

// GLUE_NO_SANITIZE_SAFESTACK
//
// Tells the SafeStack to not instrument a given function.
// See https://clang.llvm.org/docs/SafeStack.html for details.
#if GLUE_HAVE_ATTRIBUTE(no_sanitize)
    #define GLUE_NO_SANITIZE_SAFESTACK __attribute__((no_sanitize("safe-stack")))
#else
    #define GLUE_NO_SANITIZE_SAFESTACK
#endif

// GLUE_RETURNS_NONNULL
//
// Tells the compiler that a particular function never returns a null pointer.
#if GLUE_HAVE_ATTRIBUTE(returns_nonnull)
    #define GLUE_RETURNS_NONNULL __attribute__((returns_nonnull))
#else
    #define GLUE_RETURNS_NONNULL
#endif

// -----------------------------------------------------------------------------
// Variable Attributes
// -----------------------------------------------------------------------------

// GLUE_UNUSED
//
// Prevents the compiler from complaining about variables that appear unused.
//
// For code or headers that are assured to only build with C++17 and up, prefer
// just using the standard '[[maybe_unused]]' directly over this macro.
//
// Due to differences in positioning requirements between the old, compiler
// specific __attribute__ syntax and the now standard [[maybe_unused]], this
// macro does not attempt to take advantage of '[[maybe_unused]]'.
#if GLUE_HAVE_ATTRIBUTE(unused) || (defined(__GNUC__) && !defined(__clang__))
    #undef GLUE_UNUSED
    #define GLUE_UNUSED __attribute__((__unused__))
#else
    #define GLUE_UNUSED
#endif

// GLUE_PACKED
//
// Instructs the compiler not to use natural alignment for a tagged data
// structure, but instead to reduce its alignment to 1.
//
// Therefore, DO NOT APPLY THIS ATTRIBUTE TO STRUCTS CONTAINING ATOMICS. Doing
// so can cause atomic variables to be mis-aligned and silently violate
// atomicity on x86.
//
// This attribute can either be applied to members of a structure or to a
// structure in its entirety. Applying this attribute (judiciously) to a
// structure in its entirety to optimize the memory footprint of very
// commonly-used structs is fine. Do not apply this attribute to a structure in
// its entirety if the purpose is to control the offsets of the members in the
// structure. Instead, apply this attribute only to structure members that need
// it.
//
// When applying GLUE_PACKED only to specific structure members the
// natural alignment of structure members not annotated is preserved. Aligned
// member accesses are faster than non-aligned member accesses even if the
// targeted microprocessor supports non-aligned accesses.
#if GLUE_HAVE_ATTRIBUTE(packed) || (defined(__GNUC__) && !defined(__clang__))
    #define GLUE_PACKED __attribute__((__packed__))
#else
    #define GLUE_PACKED
#endif

// GLUE_DEPRECATED()
//
// Marks a deprecated class, struct, enum, function, method and variable
// declarations. The macro argument is used as a custom diagnostic message (e.g.
// suggestion of a better alternative).
//
// For code or headers that are assured to only build with C++14 and up, prefer
// just using the standard `[[deprecated("message")]]` directly over this macro.
//
// Examples:
//
//   class GLUE_DEPRECATED("Use Bar instead") Foo {...};
//
//   GLUE_DEPRECATED("Use Baz() instead") void Bar() {...}
//
//   template <typename T>
//   GLUE_DEPRECATED("Use DoThat() instead")
//   void DoThis();
//
//   enum FooEnum {
//     kBar GLUE_DEPRECATED("Use kBaz instead"),
//   };
//
// Every usage of a deprecated entity will trigger a warning when compiled with
// GCC/Clang's `-Wdeprecated-declarations` option. Google's production toolchain
// turns this warning off by default, instead relying on clang-tidy to report
// new uses of deprecated code.
#if __cplusplus > 201103L
    #define GLUE_DEPRECATED(reason) [[deprecated(reason)]]
#elif defined(__clang__)
    #define GLUE_DEPRECATED(reason) __attribute__((deprecated(reason)))
#elif defined(__GNUG__)
    #define GLUE_DEPRECATED(reason) __attribute__((deprecated))
#elif defined(_MSC_VER)
    #if _MSC_VER < 1910
        #define GLUE_DEPRECATED(reason) __declspec(deprecated)
    #else
        #define GLUE_DEPRECATED(reason) [[deprecated(reason)]]
    #endif
#endif

// GLUE_PREDICT_TRUE, GLUE_PREDICT_FALSE
//
// Enables the compiler to prioritize compilation using static analysis for
// likely paths within a boolean branch.
//
// Example:
//
//   if (GLUE_PREDICT_TRUE(expression)) {
//     return result;                        // Faster if more likely
//   } else {
//     return 0;
//   }
//
// Compilers can use the information that a certain branch is not likely to be
// taken (for instance, a CHECK failure) to optimize for the common case in
// the absence of better information (ie. compiling gcc with `-fprofile-arcs`).
//
// Recommendation: Modern CPUs dynamically predict branch execution paths,
// typically with accuracy greater than 97%. As a result, annotating every
// branch in a codebase is likely counterproductive; however, annotating
// specific branches that are both hot and consistently mispredicted is likely
// to yield performance improvements.
#if GLUE_HAVE_BUILTIN(__builtin_expect) || (defined(__GNUC__) && !defined(__clang__))
    #define GLUE_PREDICT_FALSE(x) (__builtin_expect(false || (x), false))
    #define GLUE_PREDICT_TRUE(x) (__builtin_expect(false || (x), true))
#else
    #define GLUE_PREDICT_FALSE(x) (x)
    #define GLUE_PREDICT_TRUE(x) (x)
#endif

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
    #define GLUE_ASSERT(expr)                                                                      \
        (GLUE_PREDICT_TRUE((expr)) ? static_cast<void>(0)                                          \
                                   : [] { assert(false && #expr); }()) // NOLINT
#endif

/*
 * Define GLUE_FUNC_ISA
 */
#if (defined(__GNUG__) && !defined(__clang__)) || (OCL_CLANG_VERSION > 38)
    #define GLUE_FUNC_ISA(isa) __attribute__((target(isa)))
#else
    #define GLUE_FUNC_ISA(isa)
#endif
