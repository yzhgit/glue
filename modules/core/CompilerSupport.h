//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "TargetPlatform.h"

/*
   This file provides flags for compiler features that aren't supported on all platforms.
*/

//==============================================================================
// GCC
#if GLUE_GCC
    #if (__GNUC__ * 100 + __GNUC_MINOR__) < 500
        #error "GLUE requires GCC 5.0 or later"
    #endif

    #ifndef GLUE_EXCEPTIONS_DISABLED
        #if !__EXCEPTIONS
            #define GLUE_EXCEPTIONS_DISABLED 1
        #endif
    #endif

    #define GLUE_CXX11_IS_AVAILABLE (__cplusplus >= 201103L)
    #define GLUE_CXX14_IS_AVAILABLE (__cplusplus >= 201402L)
    #define GLUE_CXX17_IS_AVAILABLE (__cplusplus >= 201703L)
#endif

//==============================================================================
// Clang
#if GLUE_CLANG
    #if (__clang_major__ < 3) || (__clang_major__ == 3 && __clang_minor__ < 4)
        #error "GLUE requires Clang 3.4 or later"
    #endif

    #ifndef GLUE_EXCEPTIONS_DISABLED
        #if !__has_feature(cxx_exceptions)
            #define GLUE_EXCEPTIONS_DISABLED 1
        #endif
    #endif

    #define GLUE_CXX11_IS_AVAILABLE (__cplusplus >= 201103L)
    #define GLUE_CXX14_IS_AVAILABLE (__cplusplus >= 201402L)
    #define GLUE_CXX17_IS_AVAILABLE (__cplusplus >= 201703L)
#endif

//==============================================================================
// MSVC
#if GLUE_MSVC
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
