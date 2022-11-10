//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

//==============================================================================
#include "glue/base/config.h"

#include "glue/base/platform.h"

#include "glue/base/compiler.h"

#include "glue/base/types.h"

#include "glue/base/macros.h"

#include "glue/base/export.h"

//==============================================================================
// Now we'll include some common OS headers..
GLUE_BEGIN_IGNORE_WARNINGS_MSVC(4514 4245 4100)

#if defined(GLUE_COMPILER_MSVC)
    #include <intrin.h>
    #include <sys/timeb.h>
    #include <windows.h>
#endif

#if defined(GLUE_COMPILER_MSVC) && defined(GLUE_DEBUG)
    #include <crtdbg.h>
    #include <debugapi.h>
#endif

GLUE_END_IGNORE_WARNINGS_MSVC

#if defined(GLUE_COMPILER_MINGW)
    #include <cstring>
    #include <sys/types.h>
#endif

#if defined(GLUE_OS_LINUX)
    #include <cstring>
    #include <signal.h>
    #include <sys/stat.h>
#endif

#if defined(GLUE_OS_ANDROID)
    #include <byteswap.h>
    #include <cstring>
#endif
