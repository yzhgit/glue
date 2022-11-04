//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

//==============================================================================
/** Current GLUE version number.

    See also glue::getVersion() for a string version.
*/
#define GLUE_MAJOR_VERSION 0
#define GLUE_MINOR_VERSION 1
#define GLUE_PATCH_VERSION 0

/** Current GLUE version number.

    Bits 16 to 32 = major version.
    Bits 8 to 16 = minor version.
    Bits 0 to 8 = point release.

    See also glue::getVersion() for a string version.
*/
#define GLUE_VERSION ((GLUE_MAJOR_VERSION << 16) + (GLUE_MINOR_VERSION << 8) + GLUE_PATCH_VERSION)

//==============================================================================
// #include <algorithm>
// #include <array>
// #include <atomic>
// #include <cassert>
// #include <cmath>
// #include <condition_variable>
// #include <cstddef>
// #include <ctime>
// #include <fstream>
// #include <functional>
// #include <iomanip>
// #include <iostream>
// #include <limits>
// #include <list>
// #include <map>
// #include <memory>
// #include <mutex>
// #include <numeric>
// #include <queue>
// #include <set>
// #include <sstream>
// #include <stdarg.h>
// #include <stdexcept>
// #include <string>
// #include <thread>
// #include <typeindex>
// #include <unordered_map>
// #include <unordered_set>
// #include <utility>
// #include <vector>

//==============================================================================
#include "glue/base/PlatformDefs.h"

//==============================================================================
// Now we'll include some common OS headers..
GLUE_BEGIN_IGNORE_WARNINGS_MSVC(4514 4245 4100)

#if GLUE_COMPILER_MSVC
    #include <intrin.h>
    #include <sys/timeb.h>
    #include <windows.h>
#endif

#if GLUE_COMPILER_MSVC && GLUE_DEBUG
    #include <crtdbg.h>
    #include <debugapi.h>
#endif

GLUE_END_IGNORE_WARNINGS_MSVC

#if GLUE_MINGW
    #include <cstring>
    #include <sys/types.h>
#endif

#if GLUE_OS_LINUX
    #include <cstring>
    #include <signal.h>
    #include <sys/stat.h>
#endif

#if GLUE_OS_ANDROID
    #include <byteswap.h>
    #include <cstring>
#endif

// undef symbols that are sometimes set by misguided 3rd-party headers..
#undef TYPE_BOOL
#undef max
#undef min
#undef major
#undef minor
#undef KeyPress

//==============================================================================
// Definitions for the int8, int16, int32, int64 types.

using uchar = unsigned char;
/** A platform-independent 8-bit signed integer type. */
using int8 = signed char;
/** A platform-independent 8-bit unsigned integer type. */
using uint8 = unsigned char;
/** A platform-independent 16-bit signed integer type. */
using int16 = signed short;
/** A platform-independent 16-bit unsigned integer type. */
using uint16 = unsigned short;
/** A platform-independent 32-bit signed integer type. */
using int32 = signed int;
/** A platform-independent 32-bit unsigned integer type. */
using uint32 = unsigned int;

#if GLUE_COMPILER_MSVC
/** A platform-independent 64-bit integer type. */
using int64 = __int64;
/** A platform-independent 64-bit unsigned integer type. */
using uint64 = unsigned __int64;
#else
/** A platform-independent 64-bit integer type. */
using int64 = long long;
/** A platform-independent 64-bit unsigned integer type. */
using uint64 = unsigned long long;
#endif

#if GLUE_64BIT
/** A signed integer type that's guaranteed to be large enough to hold a pointer without truncating
 * it. */
using pointer_sized_int = int64;
/** An unsigned integer type that's guaranteed to be large enough to hold a pointer without
 * truncating it. */
using pointer_sized_uint = uint64;
#elif GLUE_COMPILER_MSVC
/** A signed integer type that's guaranteed to be large enough to hold a pointer without truncating
 * it. */
using pointer_sized_int = _W64 int;
/** An unsigned integer type that's guaranteed to be large enough to hold a pointer without
 * truncating it. */
using pointer_sized_uint = _W64 unsigned int;
#else
/** A signed integer type that's guaranteed to be large enough to hold a pointer without truncating
 * it. */
using pointer_sized_int = int;
/** An unsigned integer type that's guaranteed to be large enough to hold a pointer without
 * truncating it. */
using pointer_sized_uint = unsigned int;
#endif

#if GLUE_OS_WINDOWS && !GLUE_MINGW
using ssize_t = pointer_sized_int;
#endif

//
// Ensure that GLUE_DLL is default unless GL_STATIC is defined
//
#if defined(_WIN32) && defined(_DLL)
    #if !defined(GLUE_DLL) && !defined(GL_STATIC)
        #define GLUE_DLL
    #endif
#endif

//
// The following block is the standard way of creating macros which make
// exporting from a DLL simpler. All files within this DLL are compiled with the
// GLUE_EXPORTS symbol defined on the command line. this symbol should not
// be defined on any project that uses this DLL. This way any other project
// whose source files include this file see GLUE_API functions as being
// imported from a DLL, wheras this DLL sees symbols defined with this macro as
// being exported.
//
#if (defined(_WIN32) || defined(_WIN32_WCE)) && defined(GLUE_DLL)
    #if defined(GLUE_EXPORTS)
        #define GLUE_API __declspec(dllexport)
    #else
        #define GLUE_API __declspec(dllimport)
    #endif
#endif

#if !defined(GLUE_API)
    #if defined(__GNUC__) && (__GNUC__ >= 4)
        #define GLUE_API __attribute__((visibility("default")))
    #else
        #define GLUE_API
    #endif
#endif

//==============================================================================
#ifndef GLUE_STRING_UTF_TYPE
    #define GLUE_STRING_UTF_TYPE 8
#endif

//==============================================================================
/** Config: GLUE_CHECK_MEMORY_LEAKS

    Enables a memory-leak check for certain objects when the app terminates. See the
   LeakedObjectDetector class and the GLUE_LEAK_DETECTOR macro for more details about enabling leak
   checking for specific classes.
*/
#if GLUE_DEBUG && !defined(GLUE_CHECK_MEMORY_LEAKS)
    #define GLUE_CHECK_MEMORY_LEAKS 1
#endif

GLUE_API bool GLUE_CALLTYPE glue_isRunningUnderDebugger() noexcept;
