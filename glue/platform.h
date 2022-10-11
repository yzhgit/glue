//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

// https://jmgorius.com/blog/2021/using-predefined-compiler-macros-c-cpp/

//==============================================================================
#if defined(_WIN32) || defined(_WIN64)
    #define GL_WINDOWS 1
#elif defined(ANDROID)
    #define GL_ANDROID 1
#elif defined(LINUX) || defined(__linux__)
    #define GL_LINUX 1
#else
    #error "Unknown platform!"
#endif

//==============================================================================
#if GL_WINDOWS
    #ifdef _MSC_VER
        #ifdef _WIN64
            #define GL_64BIT 1
        #else
            #define GL_32BIT 1
        #endif
    #endif

    #ifdef _DEBUG
        #define GL_DEBUG 1
    #endif

    #ifdef __MINGW32__
        #define GL_MINGW 1
        #ifdef __MINGW64__
            #define GL_64BIT 1
        #else
            #define GL_32BIT 1
        #endif
    #endif

    /** If defined, this indicates that the processor is little-endian. */
    #define GL_LITTLE_ENDIAN 1

    #define GL_INTEL 1
#endif

//==============================================================================
#if GL_LINUX || GL_ANDROID

    #ifdef _DEBUG
        #define GL_DEBUG 1
    #endif

    // Allow override for big-endian Linux platforms
    #if defined(__LITTLE_ENDIAN__) || !defined(GL_BIG_ENDIAN)
        #define GL_LITTLE_ENDIAN 1
        #undef GL_BIG_ENDIAN
    #else
        #undef GL_LITTLE_ENDIAN
        #define GL_BIG_ENDIAN 1
    #endif

    #if defined(__LP64__) || defined(_LP64) || defined(__arm64__)
        #define GL_64BIT 1
    #else
        #define GL_32BIT 1
    #endif

    #if defined(__arm__) || defined(__arm64__) || defined(__aarch64__)
        #define GL_ARM 1
    #elif __MMX__ || __SSE__ || __amd64__
        #define GL_INTEL 1
    #endif
#endif

//==============================================================================
// Compiler type macros.

#if defined(__clang__)
    #define GL_CLANG 1

#elif defined(__GNUC__) || defined(__GNUG__) && !defined(__clang__)
    #define GL_GCC 1

#elif defined(_MSC_VER)
    #define GL_MSVC 1

#else
    #error unknown compiler
#endif

/*
   flags for compiler features that aren't supported on all platforms.
*/

//==============================================================================
// GCC
#if GL_GCC

    #if (__GNUC__ * 100 + __GNUC_MINOR__) < 500
        #error "GLUE requires GCC 5.0 or later"
    #endif

    #ifndef GL_EXCEPTIONS_DISABLED
        #if !__EXCEPTIONS
            #define GL_EXCEPTIONS_DISABLED 1
        #endif
    #endif

#endif

//==============================================================================
// Clang
#if GL_CLANG

    #if (__clang_major__ < 3) || (__clang_major__ == 3 && __clang_minor__ < 4)
        #error "GLUE requires Clang 3.4 or later"
    #endif

    #ifndef GL_EXCEPTIONS_DISABLED
        #if !__has_feature(cxx_exceptions)
            #define GL_EXCEPTIONS_DISABLED 1
        #endif
    #endif

#endif

//==============================================================================
// MSVC
#if GL_MSVC

    #if _MSC_FULL_VER < 191025017 // VS2017
        #error "GLUE requires Visual Studio 2017 or later"
    #endif

    #ifndef GL_EXCEPTIONS_DISABLED
        #if !_CPPUNWIND
            #define GL_EXCEPTIONS_DISABLED 1
        #endif
    #endif

#endif

/** Push/pop warnings on MSVC. These macros expand to nothing on other
    compilers (like clang and gcc).
*/
#if GL_MSVC
    #define GL_IGNORE_MSVC(warnings) __pragma(warning(disable : warnings))
    #define GL_BEGIN_IGNORE_WARNINGS_LEVEL_MSVC(level, warnings)                                   \
        __pragma(warning(push, level)) GL_IGNORE_MSVC(warnings)
    #define GL_BEGIN_IGNORE_WARNINGS_MSVC(warnings) __pragma(warning(push)) GL_IGNORE_MSVC(warnings)
    #define GL_END_IGNORE_WARNINGS_MSVC __pragma(warning(pop))
#else
    #define GL_IGNORE_MSVC(warnings)
    #define GL_BEGIN_IGNORE_WARNINGS_LEVEL_MSVC(level, warnings)
    #define GL_BEGIN_IGNORE_WARNINGS_MSVC(warnings)
    #define GL_END_IGNORE_WARNINGS_MSVC
#endif

/** This macro defines the C calling convention used as the standard for JUCE calls. */
#if GL_WINDOWS
    #define GL_CALLTYPE __stdcall
    #define GL_CDECL __cdecl
#else
    #define GL_CALLTYPE
    #define GL_CDECL
#endif
