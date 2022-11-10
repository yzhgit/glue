//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#if !defined(GLUE_DEBUG) && !defined(NDEBUG)
    #define GLUE_DEBUG 1
#endif

//==============================================================================
/** Config: GLUE_LOG_ASSERTIONS

    If this flag is enabled, the jassert and jassertfalse macros will always use
   Logger::writeToLog() to write a message when an assertion happens.

    Enabling it will also leave this turned on in release builds. When it's disabled,
    however, the jassert and jassertfalse macros will not be compiled in a
    release build.

    @see jassert, jassertfalse, Logger
*/
#ifndef GLUE_LOG_ASSERTIONS
    #if GLUE_ANDROID
        #define GLUE_LOG_ASSERTIONS 1
    #else
        #define GLUE_LOG_ASSERTIONS 0
    #endif
#endif

/** Config: GLUE_CHECK_MEMORY_LEAKS

    Enables a memory-leak check for certain objects when the app terminates. See the
   LeakedObjectDetector class and the GLUE_LEAK_DETECTOR macro for more details about enabling leak
   checking for specific classes.
*/
#if defined(GLUE_DEBUG) && !defined(GLUE_CHECK_MEMORY_LEAKS)
    #define GLUE_CHECK_MEMORY_LEAKS 1
#endif

//==============================================================================
#ifndef GLUE_STRING_UTF_TYPE
    #define GLUE_STRING_UTF_TYPE 8
#endif
