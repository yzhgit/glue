//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/StandardHeader.h"

GLUE_START_NAMESPACE

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

GLUE_API const CPUInformation& getCPUInformation() noexcept;

GLUE_END_NAMESPACE
