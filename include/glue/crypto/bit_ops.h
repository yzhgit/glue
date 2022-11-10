//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/common.hpp"

namespace glue {

/**
 * If top bit of arg is set, return ~0. Otherwise return 0.
 */
template <typename T>
inline T expand_top_bit(T a)
{
    return static_cast<T>(0) - (a >> (sizeof(T) * 8 - 1));
}

/**
 * If arg is zero, return ~0. Otherwise return 0
 */
template <typename T>
inline T ct_is_zero(T x)
{
    return expand_top_bit<T>(~x & (x - 1));
}

/**
 * Power of 2 test. T should be an unsigned integer type
 * @param arg an integer value
 * @return true iff arg is 2^n for some n > 0
 */
template <typename T>
inline constexpr bool is_power_of_2(T arg)
{
    return (arg != 0) && (arg != 1) && ((arg & static_cast<T>(arg - 1)) == 0);
}

/**
 * Return the index of the highest set bit
 * T is an unsigned integer type
 * @param n an integer value
 * @return index of the highest set bit in n
 */
template <typename T>
inline size_t high_bit(T n)
{
    size_t hb = 0;

    for (size_t s = 8 * sizeof(T) / 2; s > 0; s /= 2)
    {
        const size_t z = s * ((~ct_is_zero(n >> s)) & 1);
        hb += z;
        n >>= z;
    }

    hb += n;

    return hb;
}

/**
 * Return the number of significant bytes in n
 * @param n an integer value
 * @return number of significant bytes in n
 */
template <typename T>
inline size_t significant_bytes(T n)
{
    size_t b = 0;

    for (size_t s = 8 * sizeof(n) / 2; s >= 8; s /= 2)
    {
        const size_t z = s * (~ct_is_zero(n >> s) & 1);
        b += z / 8;
        n >>= z;
    }

    b += (n != 0);

    return b;
}

/**
 * Count the trailing zero bits in n
 * @param n an integer value
 * @return maximum x st 2^x divides n
 */
template <typename T>
inline size_t ctz(T n)
{
    /*
     * If n == 0 then this function will compute 8*sizeof(T)-1, so
     * initialize lb to 1 if n == 0 to produce the expected result.
     */
    size_t lb = ct_is_zero(n) & 1;

    for (size_t s = 8 * sizeof(T) / 2; s > 0; s /= 2)
    {
        const T mask = (static_cast<T>(1) << s) - 1;
        const size_t z = s * (ct_is_zero(n & mask) & 1);
        lb += z;
        n >>= z;
    }

    return lb;
}

template <typename T>
size_t ceil_log2(T x)
{
    if (x >> (sizeof(T) * 8 - 1)) return sizeof(T) * 8;

    size_t result = 0;
    T compare = 1;

    while (compare < x)
    {
        compare <<= 1;
        result++;
    }

    return result;
}

// Potentially variable time ctz used for OCB
inline size_t var_ctz32(uint32_t n)
{
#if defined(OCL_COMPILER_GCC) || defined(OCL_COMPILER_CLANG)
    if (n == 0) return 32;
    return __builtin_ctz(n);
#else
    return ctz<uint32_t>(n);
#endif
}

} // namespace glue
