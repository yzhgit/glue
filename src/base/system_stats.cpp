//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/base/standard_header.h"

bool glue_isRunningUnderDebugger() noexcept
{
#if defined(GLUE_OS_WINDOWS)
    return IsDebuggerPresent() != FALSE;
#elif defined(GLUE_OS_BSD)
    int mib[] = {CTL_KERN, KERN_PROC, KERN_PROC_PID, ::getpid()};
    struct kinfo_proc info;
    auto infoSize = sizeof(info);
    auto result = sysctl(mib, numElementsInArray(mib), &info, &infoSize, nullptr, 0);
    return result == 0 ? ((info.ki_flag & P_TRACED) != 0) : false;
#else
    return readPosixConfigFileValue("/proc/self/status", "TracerPid").getIntValue() > 0;
#endif
}
