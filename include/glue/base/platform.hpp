//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Os
////////////////////////////////////////////////////////////////////////////////

// https://jmgorius.com/blog/2021/using-predefined-compiler-macros-c-cpp/
#if defined(_WIN32) || defined(_WIN64)
    #define GLUE_OS_WINDOWS 1
#elif defined(__ANDROID__)
    #define GLUE_OS_ANDROID 1
#elif defined(__APPLE__)
    #define GLUE_OS_MACOSX 1
#elif defined(__freebsd__) || defined(__FreeBSD__) || (__OpenBSD__)
    #define GLUE_OS_BSD 1
#elif defined(linux) || defined(__linux) || defined(__linux__)
    #define GLUE_OS_LINUX 1
#else
    #error "Unknown platform!"
#endif

////////////////////////////////////////////////////////////////////////////////
// Architectures
////////////////////////////////////////////////////////////////////////////////

#if (defined(_M_IX86) || defined(__i386__))
    #define GLUE_ARCH_X86_32 1
#endif

#if (defined(_M_X64) || defined(__x86_64__))
    #define GLUE_ARCH_X86_64 1
#endif

#if defined(GLUE_ARCH_X86_32) || defined(GLUE_ARCH_X86_64)
    #define GLUE_ARCH_X86 1
#endif

#if (defined(__arm__) || defined(_M_ARM))
    #define GLUE_ARCH_ARM 1
#endif

#if defined(__aarch64__)
    #define GLUE_ARCH_AARCH64 1
#endif

#if defined(GLUE_ARCH_AARCH64) || defined(GLUE_ARCH_ARM)
    #define GLUE_ARCH_ANY_ARM 1
#endif

//==============================================================================
#if defined(GLUE_OS_WINDOWS)
    #ifdef _MSC_VER
        #ifdef _WIN64
            #define GLUE_64BIT
        #else
            #define GLUE_32BIT
        #endif
    #endif

    #ifdef __MINGW32__
        #define GLUE_COMPILER_MINGW
        #ifdef __MINGW64__
            #define GLUE_64BIT
        #else
            #define GLUE_32BIT
        #endif
    #endif

    #define GLUE_LITTLE_ENDIAN 1
#elif defined(GLUE_OS_LINUX) || defined(GLUE_OS_ANDROID) || defined(GLUE_OS_BSD)
    #if defined(__LP64__) || defined(_LP64) || defined(__arm64__)
        #define GLUE_64BIT
    #else
        #define GLUE_32BIT
    #endif

    #include <endian.h>
    #if (__BYTE_ORDER == __LITTLE_ENDIAN)
        #define GLUE_LITTLE_ENDIAN 1
    #elif (__BYTE_ORDER == __BIG_ENDIAN)
        #define GLUE_BIG_ENDIAN 1
    #endif
#endif

/** This macro defines the C calling convention used as the standard for GLUE_ calls. */
#if defined(GLUE_OS_WINDOWS)
    #define GLUE_CALLTYPE __stdcall
    #define GLUE_CDECL __cdecl
#else
    #define GLUE_CALLTYPE
    #define GLUE_CDECL
#endif
