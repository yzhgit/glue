//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "system/SystemStats.h"

namespace glue
{

String SystemStats::getVersion()
{
    // Some basic tests, to keep an eye on things and make sure these types work ok
    // on all platforms. Let me know if any of these assertions fail on your system!
    static_assert(sizeof(pointer_sized_int) == sizeof(void*),
                  "Basic sanity test failed: please report!");
    static_assert(sizeof(int8) == 1, "Basic sanity test failed: please report!");
    static_assert(sizeof(uint8) == 1, "Basic sanity test failed: please report!");
    static_assert(sizeof(int16) == 2, "Basic sanity test failed: please report!");
    static_assert(sizeof(uint16) == 2, "Basic sanity test failed: please report!");
    static_assert(sizeof(int32) == 4, "Basic sanity test failed: please report!");
    static_assert(sizeof(uint32) == 4, "Basic sanity test failed: please report!");
    static_assert(sizeof(int64) == 8, "Basic sanity test failed: please report!");
    static_assert(sizeof(uint64) == 8, "Basic sanity test failed: please report!");

    return "GLUE v" GLUE_STRINGIFY(GLUE_MAJOR_VERSION) "." GLUE_STRINGIFY(
        GLUE_MINOR_VERSION) "." GLUE_STRINGIFY(GLUE_PATCH_VERSION);
}

#if GLUE_ANDROID && !defined(GLUE_DISABLE_GLUE_VERSION_PRINTING)
    #define GLUE_DISABLE_GLUE_VERSION_PRINTING 1
#endif

#if GLUE_DEBUG && !GLUE_DISABLE_GLUE_VERSION_PRINTING
struct GlueVersionPrinter
{
    GlueVersionPrinter() { DBG(SystemStats::getVersion()); }
};

static GlueVersionPrinter glueVersionPrinter;
#endif

//==============================================================================
struct CPUInformation
{
    CPUInformation() noexcept { initialise(); }

    void initialise() noexcept;

    int numLogicalCPUs = 0, numPhysicalCPUs = 0;

    bool hasMMX = false, hasSSE = false, hasSSE2 = false, hasSSE3 = false, has3DNow = false,
         hasFMA3 = false, hasFMA4 = false, hasSSSE3 = false, hasSSE41 = false, hasSSE42 = false,
         hasAVX = false, hasAVX2 = false, hasAVX512F = false, hasAVX512BW = false,
         hasAVX512CD = false, hasAVX512DQ = false, hasAVX512ER = false, hasAVX512IFMA = false,
         hasAVX512PF = false, hasAVX512VBMI = false, hasAVX512VL = false,
         hasAVX512VPOPCNTDQ = false, hasNeon = false;
};

static const CPUInformation& getCPUInformation() noexcept
{
    static CPUInformation info;
    return info;
}

int SystemStats::getNumCpus() noexcept { return getCPUInformation().numLogicalCPUs; }
int SystemStats::getNumPhysicalCpus() noexcept { return getCPUInformation().numPhysicalCPUs; }
bool SystemStats::hasMMX() noexcept { return getCPUInformation().hasMMX; }
bool SystemStats::has3DNow() noexcept { return getCPUInformation().has3DNow; }
bool SystemStats::hasFMA3() noexcept { return getCPUInformation().hasFMA3; }
bool SystemStats::hasFMA4() noexcept { return getCPUInformation().hasFMA4; }
bool SystemStats::hasSSE() noexcept { return getCPUInformation().hasSSE; }
bool SystemStats::hasSSE2() noexcept { return getCPUInformation().hasSSE2; }
bool SystemStats::hasSSE3() noexcept { return getCPUInformation().hasSSE3; }
bool SystemStats::hasSSSE3() noexcept { return getCPUInformation().hasSSSE3; }
bool SystemStats::hasSSE41() noexcept { return getCPUInformation().hasSSE41; }
bool SystemStats::hasSSE42() noexcept { return getCPUInformation().hasSSE42; }
bool SystemStats::hasAVX() noexcept { return getCPUInformation().hasAVX; }
bool SystemStats::hasAVX2() noexcept { return getCPUInformation().hasAVX2; }
bool SystemStats::hasAVX512F() noexcept { return getCPUInformation().hasAVX512F; }
bool SystemStats::hasAVX512BW() noexcept { return getCPUInformation().hasAVX512BW; }
bool SystemStats::hasAVX512CD() noexcept { return getCPUInformation().hasAVX512CD; }
bool SystemStats::hasAVX512DQ() noexcept { return getCPUInformation().hasAVX512DQ; }
bool SystemStats::hasAVX512ER() noexcept { return getCPUInformation().hasAVX512ER; }
bool SystemStats::hasAVX512IFMA() noexcept { return getCPUInformation().hasAVX512IFMA; }
bool SystemStats::hasAVX512PF() noexcept { return getCPUInformation().hasAVX512PF; }
bool SystemStats::hasAVX512VBMI() noexcept { return getCPUInformation().hasAVX512VBMI; }
bool SystemStats::hasAVX512VL() noexcept { return getCPUInformation().hasAVX512VL; }
bool SystemStats::hasAVX512VPOPCNTDQ() noexcept { return getCPUInformation().hasAVX512VPOPCNTDQ; }
bool SystemStats::hasNeon() noexcept { return getCPUInformation().hasNeon; }

} // namespace glue

//==============================================================================
#if GLUE_LINUX || GLUE_ANDROID
String readPosixConfigFileValue(const char* file, const char* key)
{
    StringArray lines;
    File(file).readLines(lines);

    for (int i = lines.size(); --i >= 0;) // (NB - it's important that this runs in reverse order)
        if (lines[i].upToFirstOccurrenceOf(":", false, false).trim().equalsIgnoreCase(key))
            return lines[i].fromFirstOccurrenceOf(":", false, false).trim();

    return {};
}
#endif

#if GLUE_WINDOWS
    #include "system/SystemStats_win32.cpp"
#elif GLUE_ANDROID
    #include "system/SystemStats_android.cpp"
#else
    #include "system/SystemStats_linux.cpp"
#endif
