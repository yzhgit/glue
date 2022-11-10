//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/common.hpp"

namespace glue {

/**
 * Bit rotation left by a compile-time constant amount
 * @param input the input word
 * @return input rotated left by ROT bits
 */
template <size_t ROT, typename T>
inline constexpr T rotl(T input)
{
    static_assert(ROT > 0 && ROT < 8 * sizeof(T), "Invalid rotation constant");
    return static_cast<T>((input << ROT) | (input >> (8 * sizeof(T) - ROT)));
}

/**
 * Bit rotation right by a compile-time constant amount
 * @param input the input word
 * @return input rotated right by ROT bits
 */
template <size_t ROT, typename T>
inline constexpr T rotr(T input)
{
    static_assert(ROT > 0 && ROT < 8 * sizeof(T), "Invalid rotation constant");
    return static_cast<T>((input >> ROT) | (input << (8 * sizeof(T) - ROT)));
}

/**
 * Bit rotation left, variable rotation amount
 * @param input the input word
 * @param rot the number of bits to rotate, must be between 0 and sizeof(T)*8-1
 * @return input rotated left by rot bits
 */
template <typename T>
inline T rotl_var(T input, size_t rot)
{
    return rot ? static_cast<T>((input << rot) | (input >> (sizeof(T) * 8 - rot))) : input;
}

/**
 * Bit rotation right, variable rotation amount
 * @param input the input word
 * @param rot the number of bits to rotate, must be between 0 and sizeof(T)*8-1
 * @return input rotated right by rot bits
 */
template <typename T>
inline T rotr_var(T input, size_t rot)
{
    return rot ? static_cast<T>((input >> rot) | (input << (sizeof(T) * 8 - rot))) : input;
}

#if defined(GLUE_USE_GCC_INLINE_ASM)

    #if defined(GLUE_ARCH_X86_64) || defined(GLUE_ARCH_X86_32)

template <>
inline uint32_t rotl_var(uint32_t input, size_t rot)
{
    asm("roll %1,%0" : "+r"(input) : "c"(static_cast<uint8_t>(rot)));
    return input;
}

template <>
inline uint32_t rotr_var(uint32_t input, size_t rot)
{
    asm("rorl %1,%0" : "+r"(input) : "c"(static_cast<uint8_t>(rot)));
    return input;
}

    #endif

#endif

} // namespace glue
