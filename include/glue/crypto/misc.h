//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include <iosfwd>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <vector>

namespace glue {
// Miscellaneous support functions.

// Read little endian values from unaligned storage. GCC optimizes this to
// just a single load.
inline uint16_t leget16(const void* vp)
{
    return uint16_t(((uint8_t*) vp)[1]) << 8 | ((uint8_t*) vp)[0];
}

inline uint32_t leget32(const void* vp)
{
    return (uint32_t(((uint8_t*) vp)[3]) << 24) | (uint32_t(((uint8_t*) vp)[2]) << 16) |
           (uint32_t(((uint8_t*) vp)[1]) << 8) | ((uint8_t*) vp)[0];
}

inline uint64_t leget64(const void* vp)
{
    return (uint64_t(((uint8_t*) vp)[7]) << 56) | (uint64_t(((uint8_t*) vp)[6]) << 48) |
           (uint64_t(((uint8_t*) vp)[5]) << 40) | (uint64_t(((uint8_t*) vp)[4]) << 32) |
           (uint64_t(((uint8_t*) vp)[3]) << 24) | (uint64_t(((uint8_t*) vp)[2]) << 16) |
           (uint64_t(((uint8_t*) vp)[1]) << 8) | ((uint8_t*) vp)[0];
}

inline int is_big_endian()
{
    union
    {
        uint32_t i;
        char b[4];
    } u = {0x01020304};

    return u.b[0] == 1;
}

inline uint64_t byte_swap_64(uint64_t value)
{
    // Compiled into a single bswap instruction by GCC.
    return ((value & 0xFF00000000000000u) >> 56u) | ((value & 0x00FF000000000000u) >> 40u) |
           ((value & 0x0000FF0000000000u) >> 24u) | ((value & 0x000000FF00000000u) >> 8u) |
           ((value & 0x00000000FF000000u) << 8u) | ((value & 0x0000000000FF0000u) << 24u) |
           ((value & 0x000000000000FF00u) << 40u) | ((value & 0x00000000000000FFu) << 56u);
}

// Optimized by GCC to a single mov.
inline void leput64(void* dest, uint64_t value)
{
    if (is_big_endian()) { value = byte_swap_64(value); }
    memcpy(dest, &value, sizeof(uint64_t));
}

inline uint32_t byte_swap_32(uint32_t x)
{
    // Compiled into a bswap instruction by GCC.
    return ((x & 0x000000FF) << 24) | ((x & 0x0000FF00) << 8) | ((x & 0x00FF0000) >> 8) |
           ((x & 0xFF000000) >> 24);
}

inline void leput32(void* dest, uint32_t value)
{
    if (is_big_endian()) { value = byte_swap_32(value); }
    memcpy(dest, &value, sizeof(uint32_t));
}

inline void leput16(void* dest, uint16_t value)
{
    ((uint8_t*) dest)[0] = value & 0xFF;
    ((uint8_t*) dest)[1] = value >> 8;
}

} // namespace glue
