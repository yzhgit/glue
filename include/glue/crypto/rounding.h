//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

GLUE_START_NAMESPACE

/**
 * Round up
 * @param n a non-negative integer
 * @param align_to the alignment boundary
 * @return n rounded up to a multiple of align_to
 */
inline size_t round_up(size_t n, size_t align_to)
{
    // OCL_ARG_CHECK(align_to != 0, "align_to must not be 0");

    if (n % align_to) n += align_to - (n % align_to);
    return n;
}

/**
 * Round down
 * @param n an integer
 * @param align_to the alignment boundary
 * @return n rounded down to a multiple of align_to
 */
template <typename T>
inline constexpr T round_down(T n, T align_to)
{
    return (align_to == 0) ? n : (n - (n % align_to));
}

/**
 * Clamp
 */
inline size_t clamp(size_t n, size_t lower_bound, size_t upper_bound)
{
    if (n < lower_bound) return lower_bound;
    if (n > upper_bound) return upper_bound;
    return n;
}

GLUE_END_NAMESPACE
