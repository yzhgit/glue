//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Compilers
////////////////////////////////////////////////////////////////////////////////

#if defined(__clang__)
    #define GLUE_COMPILER_CLANG
#elif defined(__GNUC__) || defined(__GNUG__) && !defined(__clang__)
    #define GLUE_COMPILER_GCC
#elif defined(_MSC_VER)
    #define GLUE_COMPILER_MSVC
#else
    #error "Unknown compiler"
#endif

////////////////////////////////////////////////////////////////////////////////
// Compiler flags
////////////////////////////////////////////////////////////////////////////////

//==============================================================================
// GCC
#if defined(GLUE_COMPILER_GCC)
    #if (__GNUC__ * 100 + __GNUC_MINOR__) < 500
        #error "GLUE requires GCC 5.0 or later"
    #endif

    #ifndef GLUE_EXCEPTIONS_DISABLED
        #if !__EXCEPTIONS
            #define GLUE_EXCEPTIONS_DISABLED 1
        #endif
    #endif

    #define GLUE_USE_GCC_INLINE_ASM

    #define GLUE_CXX11_IS_AVAILABLE (__cplusplus >= 201103L)
    #define GLUE_CXX14_IS_AVAILABLE (__cplusplus >= 201402L)
    #define GLUE_CXX17_IS_AVAILABLE (__cplusplus >= 201703L)
#endif

//==============================================================================
// Clang
#if defined(GLUE_COMPILER_CLANG)
    #if (__clang_major__ < 3) || (__clang_major__ == 3 && __clang_minor__ < 4)
        #error "GLUE requires Clang 3.4 or later"
    #endif

    #ifndef GLUE_EXCEPTIONS_DISABLED
        #if !__has_feature(cxx_exceptions)
            #define GLUE_EXCEPTIONS_DISABLED 1
        #endif
    #endif

    #define GLUE_USE_GCC_INLINE_ASM

    #define GLUE_CXX11_IS_AVAILABLE (__cplusplus >= 201103L)
    #define GLUE_CXX14_IS_AVAILABLE (__cplusplus >= 201402L)
    #define GLUE_CXX17_IS_AVAILABLE (__cplusplus >= 201703L)
#endif

//==============================================================================
// MSVC
#if defined(GLUE_COMPILER_MSVC)
    #if _MSC_FULL_VER < 191025017 // VS2017
        #error "GLUE requires Visual Studio 2017 or later"
    #endif

    #ifndef GLUE_EXCEPTIONS_DISABLED
        #if !_CPPUNWIND
            #define GLUE_EXCEPTIONS_DISABLED 1
        #endif
    #endif

    #define GLUE_CXX11_IS_AVAILABLE (_MSVC_LANG >= 201103L)
    #define GLUE_CXX14_IS_AVAILABLE (_MSVC_LANG >= 201402L)
    #define GLUE_CXX17_IS_AVAILABLE (_MSVC_LANG >= 201703L)
#endif

//==============================================================================
#if !GLUE_CXX11_IS_AVAILABLE
    #error "GLUE requires C++11 or later"
#endif

#if GLUE_CXX17_IS_AVAILABLE
    #define GLUE_NODISCARD [[nodiscard]]
#else
    #define GLUE_NODISCARD
#endif

//==============================================================================
/*  This file defines miscellaneous macros for debugging, assertions, etc.
 */

//==============================================================================

/** Push/pop warnings on MSVC. These macros expand to nothing on other
    compilers (like clang and gcc).
*/
#if defined(GLUE_COMPILER_MSVC)
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

/** Used to silence Wimplicit-fallthrough on Clang and GCC where available
    as there are a few places in the codebase where we need to do this
    deliberately and want to ignore the warning.
*/
#if defined(GLUE_COMPILER_CLANG)
    #if __has_cpp_attribute(clang::fallthrough)
        #define GLUE_FALLTHROUGH [[clang::fallthrough]];
    #else
        #define GLUE_FALLTHROUGH
    #endif
#elif defined(GLUE_COMPILER_GCC)
    #if __GNUC__ >= 7
        #define GLUE_FALLTHROUGH [[gnu::fallthrough]];
    #else
        #define GLUE_FALLTHROUGH
    #endif
#else
    #define GLUE_FALLTHROUGH
#endif

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
#if GLUE_HAVE_ATTRIBUTE(format) || defined(GLUE_COMPILER_GCC)
    #define GLUE_CHECK_FMT(string_index, first_to_check)                                           \
        __attribute__((__format__(__printf__, string_index, first_to_check)))
#else
    #define GLUE_CHECK_FMT(string_index, first_to_check)
#endif

// GLUE_ALWAYS_INLINE
//
// Forces functions to either inline or not inline. Introduced in gcc 3.1.
#if GLUE_HAVE_ATTRIBUTE(always_inline) || defined(GLUE_COMPILER_GCC)
    #define GLUE_ALWAYS_INLINE __attribute__((always_inline))
    #define GLUE_HAVE_ATTRIBUTE_ALWAYS_INLINE 1
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
#if (GLUE_HAVE_ATTRIBUTE(weak) || defined(GLUE_COMPILER_GCC)) &&                                   \
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
#if GLUE_HAVE_ATTRIBUTE(nonnull) || defined(GLUE_COMPILER_GCC)
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
#if GLUE_HAVE_ATTRIBUTE(unused) || defined(GLUE_COMPILER_GCC)
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
#if GLUE_HAVE_ATTRIBUTE(packed) || defined(GLUE_COMPILER_GCC)
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

// GLUE_LIKELY, GLUE_UNLIKELY
//
// Enables the compiler to prioritize compilation using static analysis for
// likely paths within a boolean branch.
//
// Example:
//
//   if (GLUE_LIKELY(expression)) {
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
#if GLUE_HAVE_BUILTIN(__builtin_expect) || defined(GLUE_COMPILER_GCC)
    #define GLUE_LIKELY(x) __builtin_expect(!!(x), 1)
    #define GLUE_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
    #define GLUE_LIKELY(x) (x)
    #define GLUE_UNLIKELY(x) (x)
#endif

/*
 * Define GLUE_FUNC_ISA
 * __GNUG__  = (__GNUC__ && __cplusplus)
 */
#if (defined(__GNUG__) && !defined(__clang__)) || (GLUE_CLANG_VERSION > 38)
    #define GLUE_FUNC_ISA(isa) __attribute__((target(isa)))
#else
    #define GLUE_FUNC_ISA(isa)
#endif
