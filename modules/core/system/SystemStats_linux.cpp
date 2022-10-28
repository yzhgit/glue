//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "system/SystemStats.h"

#include "maths/MathsFunctions.h"
#include "time/Time.h"

namespace glue
{

#if !GLUE_BSD
static String getCpuInfo(const char* key) { return readPosixConfigFileValue("/proc/cpuinfo", key); }

static String getLocaleValue(nl_item key)
{
    auto oldLocale = ::setlocale(LC_ALL, "");
    auto result = String::fromUTF8(nl_langinfo(key));
    ::setlocale(LC_ALL, oldLocale);
    return result;
}
#endif

//==============================================================================
void Logger::outputDebugString(const String& text) { std::cerr << text << std::endl; }

//==============================================================================
SystemStats::OperatingSystemType SystemStats::getOperatingSystemType() { return Linux; }

String SystemStats::getOperatingSystemName() { return "Linux"; }

bool SystemStats::isOperatingSystem64Bit()
{
#if GLUE_64BIT
    return true;
#else
    // xxx not sure how to find this out?..
    return false;
#endif
}

//==============================================================================
String SystemStats::getDeviceDescription()
{
#if GLUE_BSD
    int mib[] = {CTL_HW, HW_MACHINE};
    size_t machineDescriptionLength = 0;
    auto result =
        sysctl(mib, numElementsInArray(mib), nullptr, &machineDescriptionLength, nullptr, 0);

    if (result != 0 || machineDescriptionLength == 0) return {};

    MemoryBlock machineDescription{machineDescriptionLength};
    result = sysctl(mib, numElementsInArray(mib), machineDescription.getData(),
                    &machineDescriptionLength, nullptr, 0);
    return String::fromUTF8(result == 0 ? (char*) machineDescription.getData() : "");
#else
    return getCpuInfo("Hardware");
#endif
}

String SystemStats::getDeviceManufacturer() { return {}; }

String SystemStats::getCpuVendor()
{
#if GLUE_BSD
    return {};
#else
    auto v = getCpuInfo("vendor_id");

    if (v.isEmpty()) v = getCpuInfo("model name");

    return v;
#endif
}

String SystemStats::getCpuModel()
{
#if GLUE_BSD
    int mib[] = {CTL_HW, HW_MODEL};
    size_t modelLength = 0;
    auto result = sysctl(mib, numElementsInArray(mib), nullptr, &modelLength, nullptr, 0);

    if (result != 0 || modelLength == 0) return {};

    MemoryBlock model{modelLength};
    result = sysctl(mib, numElementsInArray(mib), model.getData(), &modelLength, nullptr, 0);
    return String::fromUTF8(result == 0 ? (char*) model.getData() : "");
#else
    return getCpuInfo("model name");
#endif
}

int SystemStats::getCpuSpeedInMegahertz()
{
#if GLUE_BSD
    int32 clockRate = 0;
    auto clockRateSize = sizeof(clockRate);
    auto result = sysctlbyname("hw.clockrate", &clockRate, &clockRateSize, nullptr, 0);
    return result == 0 ? clockRate : 0;
#else
    return roundToInt(getCpuInfo("cpu MHz").getFloatValue());
#endif
}

int SystemStats::getMemorySizeInMegabytes()
{
#if GLUE_BSD
    int mib[] = {CTL_HW, HW_PHYSMEM};
    int64 memory = 0;
    auto memorySize = sizeof(memory);
    auto result = sysctl(mib, numElementsInArray(mib), &memory, &memorySize, nullptr, 0);
    return result == 0 ? (int) (memory / 1e6) : 0;
#else
    struct sysinfo sysi;

    if (sysinfo(&sysi) == 0) return (int) (sysi.totalram * sysi.mem_unit / (1024 * 1024));

    return 0;
#endif
}

int SystemStats::getPageSize() { return (int) sysconf(_SC_PAGESIZE); }

//==============================================================================
String SystemStats::getLogonName()
{
    if (auto user = getenv("USER")) return String::fromUTF8(user);

    if (auto pw = getpwuid(getuid())) return String::fromUTF8(pw->pw_name);

    return {};
}

String SystemStats::getFullUserName() { return getLogonName(); }

String SystemStats::getComputerName()
{
    char name[256] = {};

    if (gethostname(name, sizeof(name) - 1) == 0) return name;

    return {};
}

//==============================================================================
void CPUInformation::initialise() noexcept
{
#if GLUE_BSD
    #if GLUE_INTEL && !GLUE_NO_INLINE_ASM
    SystemStatsHelpers::getCPUInfo(
        hasMMX, hasSSE, hasSSE2, has3DNow, hasSSE3, hasSSSE3, hasFMA3, hasSSE41, hasSSE42, hasAVX,
        hasFMA4, hasAVX2, hasAVX512F, hasAVX512DQ, hasAVX512IFMA, hasAVX512PF, hasAVX512ER,
        hasAVX512CD, hasAVX512BW, hasAVX512VL, hasAVX512VBMI, hasAVX512VPOPCNTDQ);
    #endif

    numLogicalCPUs = numPhysicalCPUs = [] {
        int mib[] = {CTL_HW, HW_NCPU};
        int32 numCPUs = 1;
        auto numCPUsSize = sizeof(numCPUs);
        auto result = sysctl(mib, numElementsInArray(mib), &numCPUs, &numCPUsSize, nullptr, 0);
        return result == 0 ? numCPUs : 1;
    }();
#else
    auto flags = getCpuInfo("flags");

    hasMMX = flags.contains("mmx");
    hasFMA3 = flags.contains("fma");
    hasFMA4 = flags.contains("fma4");
    hasSSE = flags.contains("sse");
    hasSSE2 = flags.contains("sse2");
    hasSSE3 = flags.contains("sse3");
    has3DNow = flags.contains("3dnow");
    hasSSSE3 = flags.contains("ssse3");
    hasSSE41 = flags.contains("sse4_1");
    hasSSE42 = flags.contains("sse4_2");
    hasAVX = flags.contains("avx");
    hasAVX2 = flags.contains("avx2");
    hasAVX512F = flags.contains("avx512f");
    hasAVX512BW = flags.contains("avx512bw");
    hasAVX512CD = flags.contains("avx512cd");
    hasAVX512DQ = flags.contains("avx512dq");
    hasAVX512ER = flags.contains("avx512er");
    hasAVX512IFMA = flags.contains("avx512ifma");
    hasAVX512PF = flags.contains("avx512pf");
    hasAVX512VBMI = flags.contains("avx512vbmi");
    hasAVX512VL = flags.contains("avx512vl");
    hasAVX512VPOPCNTDQ = flags.contains("avx512_vpopcntdq");

    numLogicalCPUs = getCpuInfo("processor").getIntValue() + 1;

    // Assume CPUs in all sockets have the same number of cores
    numPhysicalCPUs =
        getCpuInfo("cpu cores").getIntValue() * (getCpuInfo("physical id").getIntValue() + 1);

    if (numPhysicalCPUs <= 0) numPhysicalCPUs = numLogicalCPUs;
#endif
}

//==============================================================================
uint32 juce_millisecondsSinceStartup() noexcept
{
    return (uint32) (Time::getHighResolutionTicks() / 1000);
}

int64 Time::getHighResolutionTicks() noexcept
{
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);

    return (t.tv_sec * (int64) 1000000) + (t.tv_nsec / 1000);
}

int64 Time::getHighResolutionTicksPerSecond() noexcept
{
    return 1000000; // (microseconds)
}

double Time::getMillisecondCounterHiRes() noexcept
{
    return (double) getHighResolutionTicks() * 0.001;
}

bool Time::setSystemTimeToThisTime() const
{
    timeval t;
    t.tv_sec = decltype(timeval::tv_sec)(millisSinceEpoch / 1000);
    t.tv_usec = decltype(timeval::tv_usec)((millisSinceEpoch - t.tv_sec * 1000) * 1000);

    return settimeofday(&t, nullptr) == 0;
}

GLUE_API bool GLUE_CALLTYPE juce_isRunningUnderDebugger() noexcept
{
#if GLUE_BSD
    int mib[] = {CTL_KERN, KERN_PROC, KERN_PROC_PID, ::getpid()};
    struct kinfo_proc info;
    auto infoSize = sizeof(info);
    auto result = sysctl(mib, numElementsInArray(mib), &info, &infoSize, nullptr, 0);
    return result == 0 ? ((info.ki_flag & P_TRACED) != 0) : false;
#else
    return readPosixConfigFileValue("/proc/self/status", "TracerPid").getIntValue() > 0;
#endif
}

} // namespace glue
