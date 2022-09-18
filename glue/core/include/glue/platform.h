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

// https://jmgorius.com/blog/2021/using-predefined-compiler-macros-c-cpp/

//==============================================================================
#if defined(_WIN32) || defined(_WIN64)
    #define GL_WINDOWS 1
#elif defined(GL_ANDROID)
    #undef GL_ANDROID
    #define GL_ANDROID 1
#elif defined(__FreeBSD__) || (__OpenBSD__)
    #define GL_BSD 1
#elif defined(LINUX) || defined(__linux__)
    #define GL_LINUX 1
#elif defined(__APPLE_CPP__) || defined(__APPLE_CC__)
    #define CF_EXCLUDE_CSTD_HEADERS 1
    #include <AvailabilityMacros.h>
    #include <TargetConditionals.h> // (needed to find out what platform we're using)

    #if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
        #define GL_IPHONE 1
        #define GL_IOS 1
    #else
        #define GL_MAC 1
    #endif
#elif defined(__wasm__)
    #define GL_WASM 1
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
#if GL_MAC || GL_IOS

    #if defined(DEBUG) || defined(_DEBUG) ||                                   \
        !(defined(NDEBUG) || defined(_NDEBUG))
        #define GL_DEBUG 1
    #endif

    #if !(defined(DEBUG) || defined(_DEBUG) || defined(NDEBUG) ||              \
          defined(_NDEBUG))
        #warning                                                               \
            "Neither NDEBUG or DEBUG has been defined - you should set one of these to make it clear whether this is a release build,"
    #endif

    #ifdef __LITTLE_ENDIAN__
        #define GL_LITTLE_ENDIAN 1
    #else
        #define GL_BIG_ENDIAN 1
    #endif

    #ifdef __LP64__
        #define GL_64BIT 1
    #else
        #define GL_32BIT 1
    #endif

    #if defined(__ppc__) || defined(__ppc64__)
        #error "PowerPC is no longer supported by JUCE!"
    #elif defined(__arm__) || defined(__arm64__)
        #define GL_ARM 1
    #else
        #define GL_INTEL 1
    #endif

    #if GL_MAC
        #if !defined(MAC_OS_X_VERSION_10_14)
            #error                                                             \
                "The 10.14 SDK (Xcode 10.1+) is required to build JUCE apps. You can create apps that run on macOS 10.7+ by changing the deployment target."
        #elif MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_7
            #error "Building for OSX 10.6 is no longer supported!"
        #endif
    #endif
#endif

//==============================================================================
#if GL_LINUX || GL_ANDROID || GL_BSD

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
