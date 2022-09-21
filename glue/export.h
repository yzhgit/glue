//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

//
// Ensure that GL_DLL is default unless GL_STATIC is defined
//
#if defined(_WIN32) && defined(_DLL)
    #if !defined(GL_DLL) && !defined(GL_STATIC)
        #define GL_DLL
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
#if (defined(_WIN32) || defined(_WIN32_WCE)) && defined(GL_DLL)
    #if defined(GLUE_EXPORTS)
        #define GLUE_API __declspec(dllexport)
    #else
        #define GLUE_API __declspec(dllimport)
    #endif
#endif

#if !defined(GLUE_API)
    #if !defined(GL_NO_GCC_API_ATTRIBUTE) && defined(__GNUC__) &&              \
        (__GNUC__ >= 4)
        #define GLUE_API __attribute__((visibility("default")))
    #else
        #define GLUE_API
    #endif
#endif

//
// Automatically link Foundation library.
//
#if defined(_MSC_VER)
    #if defined(GL_DLL)
        #if defined(_DEBUG)
            #define GL_LIB_SUFFIX "d.lib"
        #else
            #define GL_LIB_SUFFIX ".lib"
        #endif
    #elif defined(_DLL)
        #if defined(_DEBUG)
            #define GL_LIB_SUFFIX "mdd.lib"
        #else
            #define GL_LIB_SUFFIX "md.lib"
        #endif
    #else
        #if defined(_DEBUG)
            #define GL_LIB_SUFFIX "mtd.lib"
        #else
            #define GL_LIB_SUFFIX "mt.lib"
        #endif
    #endif

    #if !defined(GL_NO_AUTOMATIC_LIBS) && !defined(GLUE_EXPORTS)
        #pragma comment(lib, "glue_core" GL_LIB_SUFFIX)
    #endif
#endif
