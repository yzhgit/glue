//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

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
