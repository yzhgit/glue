//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

//==============================================================================
#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

//==============================================================================
#include "glue/compiler.h"
#include "glue/platform.h"

//==============================================================================
// Now we'll include some common OS headers..
GL_BEGIN_IGNORE_WARNINGS_MSVC(4514 4245 4100)

#if GL_MSVC
    #include <intrin.h>
#endif

#if GL_LINUX
    #include <cstring>
    #include <signal.h>
#endif

#if GL_MSVC && GL_DEBUG
    #include <crtdbg.h>
#endif

GL_END_IGNORE_WARNINGS_MSVC

#if GL_MINGW
    #include <cstring>
    #include <sys/types.h>
#endif

#if GL_ANDROID
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

#include "glue/export.h"
#include "glue/macros.h"
