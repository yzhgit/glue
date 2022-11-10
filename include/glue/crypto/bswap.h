//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/common.hpp"

namespace glue {

/**
 * Swap a 16 bit integer
 */
inline uint16_t reverse_bytes(uint16_t val)
{
#if defined(GLUE_COMPILER_GCC) || defined(GLUE_COMPILER_CLANG)
    return __builtin_bswap16(val);
#else
    return static_cast<uint16_t>((val << 8) | (val >> 8));
#endif
}

/**
 * Swap a 32 bit integer
 */
inline uint32_t reverse_bytes(uint32_t val)
{
#if defined(GLUE_COMPILER_GCC) || defined(GLUE_COMPILER_CLANG)
    return __builtin_bswap32(val);

#elif defined(GLUE_COMPILER_MSVC)
    return _byteswap_ulong(val);

#elif defined(GLUE_USE_GCC_INLINE_ASM) && defined(GLUE_ARCH_X86)

    // GCC-style inline assembly for x86 or x86-64
    asm("bswapl %0" : "=r"(val) : "0"(val));
    return val;

#else
    // Generic implementation
    uint16_t hi = static_cast<uint16_t>(val >> 16);
    uint16_t lo = static_cast<uint16_t>(val);

    hi = reverse_bytes(hi);
    lo = reverse_bytes(lo);

    return (static_cast<uint32_t>(lo) << 16) | hi;
#endif
}

/**
 * Swap a 64 bit integer
 */
inline uint64_t reverse_bytes(uint64_t val)
{
#if defined(GLUE_COMPILER_GCC) || defined(GLUE_COMPILER_CLANG)
    return __builtin_bswap64(val);

#elif defined(GLUE_COMPILER_MSVC)
    return _byteswap_uint64(val);

#elif defined(GLUE_USE_GCC_INLINE_ASM) && defined(GLUE_ARCH_X86_64)
    // GCC-style inline assembly for x86-64
    asm("bswapq %0" : "=r"(val) : "0"(val));
    return val;

#else
    /* Generic implementation. Defined in terms of 32-bit bswap so any
     * optimizations in that version can help.
     */

    uint32_t hi = static_cast<uint32_t>(val >> 32);
    uint32_t lo = static_cast<uint32_t>(val);

    hi = reverse_bytes(hi);
    lo = reverse_bytes(lo);

    return (static_cast<uint64_t>(lo) << 32) | hi;
#endif
}

/**
 * Swap 4 Ts in an array
 */
template <typename T>
inline void bswap_4(T x[4])
{
    x[0] = reverse_bytes(x[0]);
    x[1] = reverse_bytes(x[1]);
    x[2] = reverse_bytes(x[2]);
    x[3] = reverse_bytes(x[3]);
}

} // namespace glue
