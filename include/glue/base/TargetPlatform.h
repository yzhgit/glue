//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

// https://jmgorius.com/blog/2021/using-predefined-compiler-macros-c-cpp/

//==============================================================================
#if defined(_WIN32) || defined(_WIN64)
    #define GLUE_WINDOWS 1
#elif defined(__ANDROID__)
    #define GLUE_ANDROID 1
#elif defined(__FreeBSD__) || (__OpenBSD__)
    #define GLUE_BSD 1
#elif defined(linux) || defined(__linux) || defined(__linux__)
    #define GLUE_LINUX 1
#else
    #error "Unknown platform!"
#endif

//==============================================================================
#if GLUE_WINDOWS
    #ifdef _MSC_VER
        #ifdef _WIN64
            #define GLUE_64BIT 1
        #else
            #define GLUE_32BIT 1
        #endif
    #endif

    #ifdef _DEBUG
        #define GLUE_DEBUG 1
    #endif

    #ifdef __MINGW32__
        #define GLUE_MINGW 1
        #ifdef __MINGW64__
            #define GLUE_64BIT 1
        #else
            #define GLUE_32BIT 1
        #endif
    #endif

    /** If defined, this indicates that the processor is little-endian. */
    #define GLUE_LITTLE_ENDIAN 1

    #define GLUE_INTEL 1
#endif

//==============================================================================
#if GLUE_LINUX || GLUE_ANDROID
    #ifdef _DEBUG
        #define GLUE_DEBUG 1
    #endif

    // Allow override for big-endian Linux platforms
    #if defined(__LITTLE_ENDIAN__) || !defined(GLUE_BIG_ENDIAN)
        #define GLUE_LITTLE_ENDIAN 1
        #undef GLUE_BIG_ENDIAN
    #else
        #undef GLUE_LITTLE_ENDIAN
        #define GLUE_BIG_ENDIAN 1
    #endif

    #if defined(__LP64__) || defined(_LP64) || defined(__arm64__)
        #define GLUE_64BIT 1
    #else
        #define GLUE_32BIT 1
    #endif

    #if defined(__arm__) || defined(__arm64__) || defined(__aarch64__)
        #define GL_ARM 1
    #elif __MMX__ || __SSE__ || __amd64__
        #define GLUE_INTEL 1
    #endif
#endif

//==============================================================================
// Compiler type macros.

#if defined(__clang__)
    #define GLUE_CLANG 1
#elif defined(__GNUC__) || defined(__GNUG__) && !defined(__clang__)
    #define GLUE_GCC 1
#elif defined(_MSC_VER)
    #define GLUE_MSVC 1
#else
    #error "Unknown compiler"
#endif
