//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include <stdint.h>

GLUE_START_NAMESPACE

#if defined(__SIZEOF_INT128__) && defined(GLUE_64BIT)
    #define GLUE_TARGET_HAS_NATIVE_UINT128

    // Prefer TI mode over __int128 as GCC rejects the latter in pendantic mode
    #if defined(__GNUG__)
typedef unsigned int uint128_t __attribute__((mode(TI)));
    #else
typedef unsigned __int128 uint128_t;
    #endif
#endif

GLUE_END_NAMESPACE

#if defined(GLUE_TARGET_HAS_NATIVE_UINT128)

    #define GLUE_FAST_64X64_MUL(a, b, lo, hi)                                                      \
        do {                                                                                       \
            const uint128_t r = static_cast<uint128_t>(a) * b;                                     \
            *hi = (r >> 64) & 0xFFFFFFFFFFFFFFFF;                                                  \
            *lo = (r) &0xFFFFFFFFFFFFFFFF;                                                         \
        } while (0)

#elif defined(GLUE_COMPILER_MSVC) && defined(GLUE_64BIT)

    #include <intrin.h>
    #pragma intrinsic(_umul128)

    #define GLUE_FAST_64X64_MUL(a, b, lo, hi)                                                      \
        do {                                                                                       \
            *lo = _umul128(a, b, hi);                                                              \
        } while (0)

#elif defined(GLUE_USE_GCC_INLINE_ASM)

    #if defined(GLUE_ARCH_X86_64)

        #define GLUE_FAST_64X64_MUL(a, b, lo, hi)                                                  \
            do {                                                                                   \
                asm("mulq %3" : "=d"(*hi), "=a"(*lo) : "a"(a), "rm"(b) : "cc");                    \
            } while (0)

    #elif defined(GLUE_ARCH_ALPHA)

        #define GLUE_FAST_64X64_MUL(a, b, lo, hi)                                                  \
            do {                                                                                   \
                asm("umulh %1,%2,%0" : "=r"(*hi) : "r"(a), "r"(b));                                \
                *lo = a * b;                                                                       \
            } while (0)

    #elif defined(GLUE_ARCH_IA64)

        #define GLUE_FAST_64X64_MUL(a, b, lo, hi)                                                  \
            do {                                                                                   \
                asm("xmpy.hu %0=%1,%2" : "=f"(*hi) : "f"(a), "f"(b));                              \
                *lo = a * b;                                                                       \
            } while (0)

    #elif defined(GLUE_ARCH_PPC64)

        #define GLUE_FAST_64X64_MUL(a, b, lo, hi)                                                  \
            do {                                                                                   \
                asm("mulhdu %0,%1,%2" : "=r"(*hi) : "r"(a), "r"(b) : "cc");                        \
                *lo = a * b;                                                                       \
            } while (0)

    #endif

#endif

GLUE_START_NAMESPACE

/**
 * Perform a 64x64->128 bit multiplication
 */
inline void mul64x64_128(uint64_t a, uint64_t b, uint64_t* lo, uint64_t* hi)
{
#if defined(GLUE_FAST_64X64_MUL)
    GLUE_FAST_64X64_MUL(a, b, lo, hi);
#else

    /*
     * Do a 64x64->128 multiply using four 32x32->64 multiplies plus
     * some adds and shifts. Last resort for CPUs like UltraSPARC (with
     * 64-bit registers/ALU, but no 64x64->128 multiply) or 32-bit CPUs.
     */
    const size_t HWORD_BITS = 32;
    const uint32_t HWORD_MASK = 0xFFFFFFFF;

    const uint32_t a_hi = (a >> HWORD_BITS);
    const uint32_t a_lo = (a & HWORD_MASK);
    const uint32_t b_hi = (b >> HWORD_BITS);
    const uint32_t b_lo = (b & HWORD_MASK);

    uint64_t x0 = static_cast<uint64_t>(a_hi) * b_hi;
    uint64_t x1 = static_cast<uint64_t>(a_lo) * b_hi;
    uint64_t x2 = static_cast<uint64_t>(a_hi) * b_lo;
    uint64_t x3 = static_cast<uint64_t>(a_lo) * b_lo;

    // this cannot overflow as (2^32-1)^2 + 2^32-1 < 2^64-1
    x2 += x3 >> HWORD_BITS;

    // this one can overflow
    x2 += x1;

    // propagate the carry if any
    x0 += static_cast<uint64_t>(static_cast<bool>(x2 < x1)) << HWORD_BITS;

    *hi = x0 + (x2 >> HWORD_BITS);
    *lo = ((x2 & HWORD_MASK) << HWORD_BITS) + (x3 & HWORD_MASK);
#endif
}

GLUE_END_NAMESPACE
