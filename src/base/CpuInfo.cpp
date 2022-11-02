//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/base/CpuInfo.h"

#include "glue/base/HeapBlock.h"

namespace glue
{

static const CPUInformation& getCPUInformation() noexcept
{
    static CPUInformation info;
    return info;
}

#if GLUE_WINDOWS

    #if GLUE_MINGW || GLUE_CLANG
static void callCPUID(int result[4], uint32 type)
{
    uint32 la = (uint32) result[0], lb = (uint32) result[1], lc = (uint32) result[2],
           ld = (uint32) result[3];

    asm("mov %%ebx, %%esi \n\t"
        "cpuid \n\t"
        "xchg %%esi, %%ebx"
        : "=a"(la), "=S"(lb), "=c"(lc), "=d"(ld)
        : "a"(type)
        #if GLUE_64BIT
              ,
          "b"(lb), "c"(lc), "d"(ld)
        #endif
    );

    result[0] = (int) la;
    result[1] = (int) lb;
    result[2] = (int) lc;
    result[3] = (int) ld;
}
    #else
static void callCPUID(int result[4], int infoType) { __cpuid(result, infoType); }
    #endif

static std::string getCpuVendor()
{
    int info[4] = {0};
    callCPUID(info, 0);

    char v[12];
    memcpy(v, info + 1, 4);
    memcpy(v + 4, info + 3, 4);
    memcpy(v + 8, info + 2, 4);

    return std::string(v, 12);
}

static std::string getCpuModel()
{
    char name[65] = {0};
    int info[4] = {0};

    callCPUID(info, 0x80000000);

    const int numExtIDs = info[0];

    if ((unsigned) numExtIDs < 0x80000004) // if brand string is unsupported
        return {};

    callCPUID(info, 0x80000002);
    memcpy(name, info, sizeof(info));

    callCPUID(info, 0x80000003);
    memcpy(name + 16, info, sizeof(info));

    callCPUID(info, 0x80000004);
    memcpy(name + 32, info, sizeof(info));

    return std::string(name);
}

static int findNumberOfPhysicalCores() noexcept
{
    #if GLUE_MINGW
    // Not implemented in MinGW
    jassertfalse;

    return 1;
    #else

    int numPhysicalCores = 0;
    DWORD bufferSize = 0;
    GetLogicalProcessorInformation(nullptr, &bufferSize);

    if (auto numBuffers = (size_t) (bufferSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION)))
    {
        HeapBlock<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> buffer(numBuffers);

        if (GetLogicalProcessorInformation(buffer, &bufferSize))
            for (size_t i = 0; i < numBuffers; ++i)
                if (buffer[i].Relationship == RelationProcessorCore) ++numPhysicalCores;
    }

    return numPhysicalCores;
    #endif // GLUE_MINGW
}

//==============================================================================
void CPUInformation::initialise() noexcept
{
    int info[4] = {0};
    callCPUID(info, 1);

    // NB: IsProcessorFeaturePresent doesn't work on XP
    hasMMX = (info[3] & (1 << 23)) != 0;
    hasSSE = (info[3] & (1 << 25)) != 0;
    hasSSE2 = (info[3] & (1 << 26)) != 0;
    hasSSE3 = (info[2] & (1 << 0)) != 0;
    hasAVX = (info[2] & (1 << 28)) != 0;
    hasFMA3 = (info[2] & (1 << 12)) != 0;
    hasSSSE3 = (info[2] & (1 << 9)) != 0;
    hasSSE41 = (info[2] & (1 << 19)) != 0;
    hasSSE42 = (info[2] & (1 << 20)) != 0;

    has3DNow = (info[1] & (1 << 31)) != 0;

    callCPUID(info, 0x80000001);
    hasFMA4 = (info[2] & (1 << 16)) != 0;

    callCPUID(info, 7);

    hasAVX2 = ((unsigned int) info[1] & (1 << 5)) != 0;
    hasAVX512F = ((unsigned int) info[1] & (1u << 16)) != 0;
    hasAVX512DQ = ((unsigned int) info[1] & (1u << 17)) != 0;
    hasAVX512IFMA = ((unsigned int) info[1] & (1u << 21)) != 0;
    hasAVX512PF = ((unsigned int) info[1] & (1u << 26)) != 0;
    hasAVX512ER = ((unsigned int) info[1] & (1u << 27)) != 0;
    hasAVX512CD = ((unsigned int) info[1] & (1u << 28)) != 0;
    hasAVX512BW = ((unsigned int) info[1] & (1u << 30)) != 0;
    hasAVX512VL = ((unsigned int) info[1] & (1u << 31)) != 0;
    hasAVX512VBMI = ((unsigned int) info[2] & (1u << 1)) != 0;
    hasAVX512VPOPCNTDQ = ((unsigned int) info[2] & (1u << 14)) != 0;

    SYSTEM_INFO systemInfo;
    GetNativeSystemInfo(&systemInfo);
    numLogicalCPUs = (int) systemInfo.dwNumberOfProcessors;
    numPhysicalCPUs = findNumberOfPhysicalCores();

    if (numPhysicalCPUs <= 0) numPhysicalCPUs = numLogicalCPUs;
}

#elif GLUE_ANDROID

static std::string getCpuVendor() { return AndroidStatsHelpers::getSystemProperty("os.arch"); }

static std::string getCpuModel() { return readPosixConfigFileValue("/proc/cpuinfo", "Hardware"); }

void CPUInformation::initialise() noexcept
{
    numPhysicalCPUs = numLogicalCPUs = jmax((int) 1, (int) android_getCpuCount());

    auto cpuFamily = android_getCpuFamily();
    auto cpuFeatures = android_getCpuFeatures();

    if (cpuFamily == ANDROID_CPU_FAMILY_X86 || cpuFamily == ANDROID_CPU_FAMILY_X86_64)
    {
        hasMMX = hasSSE = hasSSE2 = (cpuFamily == ANDROID_CPU_FAMILY_X86_64);

        hasSSSE3 = ((cpuFeatures & ANDROID_CPU_X86_FEATURE_SSSE3) != 0);
        hasSSE41 = ((cpuFeatures & ANDROID_CPU_X86_FEATURE_SSE4_1) != 0);
        hasSSE42 = ((cpuFeatures & ANDROID_CPU_X86_FEATURE_SSE4_2) != 0);
        hasAVX = ((cpuFeatures & ANDROID_CPU_X86_FEATURE_AVX) != 0);
        hasAVX2 = ((cpuFeatures & ANDROID_CPU_X86_FEATURE_AVX2) != 0);

        // Google does not distinguish between MMX, SSE, SSE2, SSE3 and SSSE3. So
        // I assume (and quick Google searches seem to confirm this) that there are
        // only devices out there that either support all of this or none of this.
        if (hasSSSE3) hasMMX = hasSSE = hasSSE2 = hasSSE3 = true;
    }
    else if (cpuFamily == ANDROID_CPU_FAMILY_ARM)
    {
        hasNeon = ((cpuFeatures & ANDROID_CPU_ARM_FEATURE_NEON) != 0);
    }
    else if (cpuFamily == ANDROID_CPU_FAMILY_ARM64)
    {
        // all arm 64-bit cpus have neon
        hasNeon = true;
    }
}
#else

static std::string getCpuInfo(const char* key)
{
    return readPosixConfigFileValue("/proc/cpuinfo", key);
}

void CPUInformation::initialise() noexcept
{
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
}
#endif

} // namespace glue
