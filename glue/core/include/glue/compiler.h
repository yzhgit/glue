/*
 * Copyright 2015 zhangyao<mosee.gd@163.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "glue/config.h"

// -----------------------------------------------------------------------------
// Compiler Feature Checks
// -----------------------------------------------------------------------------

// GL_HAVE_BUILTIN()
//
// Checks whether the compiler supports a Clang Feature Checking Macro, and if
// so, checks whether it supports the provided builtin function "x" where x
// is one of the functions noted in
// https://clang.llvm.org/docs/LanguageExtensions.html
//
// Note: Use this macro to avoid an extra level of #ifdef __has_builtin check.
// http://releases.llvm.org/3.3/tools/clang/docs/LanguageExtensions.html
#ifdef __has_builtin
    #define GL_HAVE_BUILTIN(x) __has_builtin(x)
#else
    #define GL_HAVE_BUILTIN(x) 0
#endif

#ifdef __has_feature
    #define GL_HAVE_FEATURE(f) __has_feature(f)
#else
    #define GL_HAVE_FEATURE(f) 0
#endif

// GL_HAVE_ATTRIBUTE
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
    #define GL_HAVE_ATTRIBUTE(x) __has_attribute(x)
#else
    #define GL_HAVE_ATTRIBUTE(x) 0
#endif

// GL_HAVE_CPP_ATTRIBUTE
//
// A function-like feature checking macro that accepts C++11 style attributes.
// It's a wrapper around `__has_cpp_attribute`, defined by ISO C++ SD-6
// (https://en.cppreference.com/w/cpp/experimental/feature_test). If we don't
// find `__has_cpp_attribute`, will evaluate to 0.
#if defined(__cplusplus) && defined(__has_cpp_attribute)
    // NOTE: requiring __cplusplus above should not be necessary, but
    // works around https://bugs.llvm.org/show_bug.cgi?id=23435.
    #define GL_HAVE_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
    #define GL_HAVE_CPP_ATTRIBUTE(x) 0
#endif

// -----------------------------------------------------------------------------
// Function Attributes
// -----------------------------------------------------------------------------
//
// GCC: https://gcc.gnu.org/onlinedocs/gcc/Function-Attributes.html
// Clang: https://clang.llvm.org/docs/AttributeReference.html

// GL_CHECK_FMT
//
// Tells the compiler to perform `printf` format string checking if the
// compiler supports it; see the 'format' attribute in
// <https://gcc.gnu.org/onlinedocs/gcc-4.7.0/gcc/Function-Attributes.html>.
//
// Note: As the GCC manual states, "[s]ince non-static C++ methods
// have an implicit 'this' argument, the arguments of such methods
// should be counted from two, not one."
#if GL_HAVE_ATTRIBUTE(format) || (defined(__GNUC__) && !defined(__clang__))
    #define GL_CHECK_FMT(string_index, first_to_check)                         \
        __attribute__((__format__(__printf__, string_index, first_to_check)))
#else
    #define GL_CHECK_FMT(string_index, first_to_check)
#endif

// GL_ALWAYS_INLINE
//
// Forces functions to either inline or not inline. Introduced in gcc 3.1.
#if GL_HAVE_ATTRIBUTE(always_inline) ||                                        \
    (defined(__GNUC__) && !defined(__clang__))
    #define GL_ALWAYS_INLINE __attribute__((always_inline))
    #define GL_HAVE_ATTRIBUTE_ALWAYS_INLINE 1
#else
    #define GL_ALWAYS_INLINE
#endif

// GL_WEAK
//
// Tags a function as weak for the purposes of compilation and linking.
// Weak attributes did not work properly in LLVM's Windows backend before
// 9.0.0, so disable them there. See https://bugs.llvm.org/show_bug.cgi?id=37598
// for further information.
// The MinGW compiler doesn't complain about the weak attribute until the link
// step, presumably because Windows doesn't use ELF binaries.
#if (GL_HAVE_ATTRIBUTE(weak) || (defined(__GNUC__) && !defined(__clang__))) && \
    (!defined(_WIN32) || (defined(__clang__) && __clang_major__ >= 9)) &&      \
    !defined(__MINGW32__)
    #undef GL_WEAK
    #define GL_WEAK __attribute__((weak))
    #define GL_HAVE_ATTRIBUTE_WEAK 1
#else
    #define GL_WEAK
    #define GL_HAVE_ATTRIBUTE_WEAK 0
#endif

// GL_NONNULL
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
//   void Function(void* arg_a, void* arg_b) GL_NONNULL(1);
//
//   class C {
//     /* arg_a cannot be null, but arg_b can */
//     void Method(void* arg_a, void* arg_b) GL_NONNULL(2);
//
//     /* arg_a cannot be null, but arg_b can */
//     static void StaticMethod(void* arg_a, void* arg_b)
//     GL_NONNULL(1);
//   };
//
// If no arguments are provided, then all pointer arguments should be non-null.
//
//  /* No pointer arguments may be null. */
//  void Function(void* arg_a, void* arg_b, int arg_c) GL_NONNULL();
//
// NOTE: The GCC nonnull attribute actually accepts a list of arguments, but
// GL_NONNULL does not.
#if GL_HAVE_ATTRIBUTE(nonnull) || (defined(__GNUC__) && !defined(__clang__))
    #define GL_NONNULL(arg_index) __attribute__((nonnull(arg_index)))
#else
    #define GL_NONNULL(...)
#endif

// GL_NO_SANITIZE_ADDRESS
//
// Tells the AddressSanitizer (or other memory testing tools) to ignore a given
// function. Useful for cases when a function reads random locations on stack,
// calls _exit from a cloned subprocess, deliberately accesses buffer
// out of bounds or does other scary things with memory.
// NOTE: GCC supports AddressSanitizer(asan) since 4.8.
// https://gcc.gnu.org/gcc-4.8/changes.html
#if GL_HAVE_ATTRIBUTE(no_sanitize_address)
    #define GL_NO_SANITIZE_ADDRESS __attribute__((no_sanitize_address))
#elif defined(_MSC_VER) && _MSC_VER >= 1928
    // https://docs.microsoft.com/en-us/cpp/cpp/no-sanitize-address
    #define GL_NO_SANITIZE_ADDRESS __declspec(no_sanitize_address)
#else
    #define GL_NO_SANITIZE_ADDRESS
#endif

// GL_NO_SANITIZE_MEMORY
//
// Tells the MemorySanitizer to relax the handling of a given function. All "Use
// of uninitialized value" warnings from such functions will be suppressed, and
// all values loaded from memory will be considered fully initialized.  This
// attribute is similar to the GL_NO_SANITIZE_ADDRESS attribute
// above, but deals with initialized-ness rather than addressability issues.
// NOTE: MemorySanitizer(msan) is supported by Clang but not GCC.
#if GL_HAVE_ATTRIBUTE(no_sanitize_memory)
    #define GL_NO_SANITIZE_MEMORY __attribute__((no_sanitize_memory))
#else
    #define GL_NO_SANITIZE_MEMORY
#endif

// GL_NO_SANITIZE_THREAD
//
// Tells the ThreadSanitizer to not instrument a given function.
// NOTE: GCC supports ThreadSanitizer(tsan) since 4.8.
// https://gcc.gnu.org/gcc-4.8/changes.html
#if GL_HAVE_ATTRIBUTE(no_sanitize_thread)
    #define GL_NO_SANITIZE_THREAD __attribute__((no_sanitize_thread))
#else
    #define GL_NO_SANITIZE_THREAD
#endif

// GL_NO_SANITIZE_UNDEFINED
//
// Tells the UndefinedSanitizer to ignore a given function. Useful for cases
// where certain behavior (eg. division by zero) is being used intentionally.
// NOTE: GCC supports UndefinedBehaviorSanitizer(ubsan) since 4.9.
// https://gcc.gnu.org/gcc-4.9/changes.html
#if GL_HAVE_ATTRIBUTE(no_sanitize_undefined)
    #define GL_NO_SANITIZE_UNDEFINED __attribute__((no_sanitize_undefined))
#elif GL_HAVE_ATTRIBUTE(no_sanitize)
    #define GL_NO_SANITIZE_UNDEFINED __attribute__((no_sanitize("undefined")))
#else
    #define GL_NO_SANITIZE_UNDEFINED
#endif

// GL_NO_SANITIZE_CFI
//
// Tells the ControlFlowIntegrity sanitizer to not instrument a given function.
// See https://clang.llvm.org/docs/ControlFlowIntegrity.html for details.
#if GL_HAVE_ATTRIBUTE(no_sanitize)
    #define GL_NO_SANITIZE_CFI __attribute__((no_sanitize("cfi")))
#else
    #define GL_NO_SANITIZE_CFI
#endif

// GL_NO_SANITIZE_SAFESTACK
//
// Tells the SafeStack to not instrument a given function.
// See https://clang.llvm.org/docs/SafeStack.html for details.
#if GL_HAVE_ATTRIBUTE(no_sanitize)
    #define GL_NO_SANITIZE_SAFESTACK __attribute__((no_sanitize("safe-stack")))
#else
    #define GL_NO_SANITIZE_SAFESTACK
#endif

// GL_RETURNS_NONNULL
//
// Tells the compiler that a particular function never returns a null pointer.
#if GL_HAVE_ATTRIBUTE(returns_nonnull)
    #define GL_RETURNS_NONNULL __attribute__((returns_nonnull))
#else
    #define GL_RETURNS_NONNULL
#endif

// -----------------------------------------------------------------------------
// Variable Attributes
// -----------------------------------------------------------------------------

// GL_UNUSED
//
// Prevents the compiler from complaining about variables that appear unused.
//
// For code or headers that are assured to only build with C++17 and up, prefer
// just using the standard '[[maybe_unused]]' directly over this macro.
//
// Due to differences in positioning requirements between the old, compiler
// specific __attribute__ syntax and the now standard [[maybe_unused]], this
// macro does not attempt to take advantage of '[[maybe_unused]]'.
#if GL_HAVE_ATTRIBUTE(unused) || (defined(__GNUC__) && !defined(__clang__))
    #undef GL_UNUSED
    #define GL_UNUSED __attribute__((__unused__))
#else
    #define GL_UNUSED
#endif

// GL_PACKED
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
// When applying GL_PACKED only to specific structure members the
// natural alignment of structure members not annotated is preserved. Aligned
// member accesses are faster than non-aligned member accesses even if the
// targeted microprocessor supports non-aligned accesses.
#if GL_HAVE_ATTRIBUTE(packed) || (defined(__GNUC__) && !defined(__clang__))
    #define GL_PACKED __attribute__((__packed__))
#else
    #define GL_PACKED
#endif

// GL_DEPRECATED()
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
//   class GL_DEPRECATED("Use Bar instead") Foo {...};
//
//   GL_DEPRECATED("Use Baz() instead") void Bar() {...}
//
//   template <typename T>
//   GL_DEPRECATED("Use DoThat() instead")
//   void DoThis();
//
//   enum FooEnum {
//     kBar GL_DEPRECATED("Use kBaz instead"),
//   };
//
// Every usage of a deprecated entity will trigger a warning when compiled with
// GCC/Clang's `-Wdeprecated-declarations` option. Google's production toolchain
// turns this warning off by default, instead relying on clang-tidy to report
// new uses of deprecated code.
#if GL_HAVE_ATTRIBUTE(deprecated)
    #define GL_DEPRECATED(message) __attribute__((deprecated(message)))
#else
    #define GL_DEPRECATED(message)
#endif

// GL_LIKELY, GL_UNLIKELY
//
// Enables the compiler to prioritize compilation using static analysis for
// likely paths within a boolean branch.
//
// Example:
//
//   if (GL_LIKELY(expression)) {
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
#if GL_HAVE_BUILTIN(__builtin_expect) ||                                       \
    (defined(__GNUC__) && !defined(__clang__))
    #define GL_UNLIKELY(x) (__builtin_expect(false || (x), false))
    #define GL_LIKELY(x) (__builtin_expect(false || (x), true))
#else
    #define GL_UNLIKELY(x) (x)
    #define GL_LIKELY(x) (x)
#endif
