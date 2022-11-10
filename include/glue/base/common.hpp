//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

//==============================================================================
#include "glue/base/config.hpp"

#include "glue/base/platform.hpp"

#include "glue/base/compiler.hpp"

#include "glue/base/types.hpp"

#include "glue/base/macros.hpp"

#include "glue/base/export.hpp"

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
