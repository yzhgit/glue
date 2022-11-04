//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/StandardHeader.h"

GLUE_START_NAMESPACE

//==============================================================================
/** Contains static methods for converting the byte order between different
    endiannesses.

    @tags{Core}
*/
class ByteOrder
{
public:
    //==============================================================================
    /** Swaps the upper and lower bytes of a 16-bit integer. */
    constexpr static uint16_t swap(uint16_t value) noexcept;

    /** Swaps the upper and lower bytes of a 16-bit integer. */
    constexpr static int16_t swap(int16_t value) noexcept;

    /** Reverses the order of the 4 bytes in a 32-bit integer. */
    static uint32_t swap(uint32_t value) noexcept;

    /** Reverses the order of the 4 bytes in a 32-bit integer. */
    static int32_t swap(int32_t value) noexcept;

    /** Reverses the order of the 8 bytes in a 64-bit integer. */
    static uint64_t swap(uint64_t value) noexcept;

    /** Reverses the order of the 8 bytes in a 64-bit integer. */
    static int64_t swap(int64_t value) noexcept;

    /** Returns a garbled float which has the reverse byte-order of the original. */
    static float swap(float value) noexcept;

    /** Returns a garbled double which has the reverse byte-order of the original. */
    static double swap(double value) noexcept;

    //==============================================================================
    /** Swaps the byte order of a signed or unsigned integer if the CPU is big-endian */
    template <typename Type>
    static Type swapIfBigEndian(Type value) noexcept
    {
#if defined(GLUE_LITTLE_ENDIAN)
        return value;
#else
        return swap(value);
#endif
    }

    /** Swaps the byte order of a signed or unsigned integer if the CPU is little-endian */
    template <typename Type>
    static Type swapIfLittleEndian(Type value) noexcept
    {
#if defined(GLUE_LITTLE_ENDIAN)
        return swap(value);
#else
        return value;
#endif
    }

    //==============================================================================
    /** Turns 4 bytes into a little-endian integer. */
    constexpr static uint32_t littleEndianInt(const void* bytes) noexcept;

    /** Turns 8 bytes into a little-endian integer. */
    constexpr static uint64_t littleEndianInt64(const void* bytes) noexcept;

    /** Turns 2 bytes into a little-endian integer. */
    constexpr static uint16_t littleEndianShort(const void* bytes) noexcept;

    /** Converts 3 little-endian bytes into a signed 24-bit value (which is sign-extended to 32
     * bits). */
    constexpr static int littleEndian24Bit(const void* bytes) noexcept;

    /** Copies a 24-bit number to 3 little-endian bytes. */
    static void littleEndian24BitToChars(int32_t value, void* destBytes) noexcept;

    //==============================================================================
    /** Turns 4 bytes into a big-endian integer. */
    constexpr static uint32_t bigEndianInt(const void* bytes) noexcept;

    /** Turns 8 bytes into a big-endian integer. */
    constexpr static uint64_t bigEndianInt64(const void* bytes) noexcept;

    /** Turns 2 bytes into a big-endian integer. */
    constexpr static uint16_t bigEndianShort(const void* bytes) noexcept;

    /** Converts 3 big-endian bytes into a signed 24-bit value (which is sign-extended to 32 bits).
     */
    constexpr static int bigEndian24Bit(const void* bytes) noexcept;

    /** Copies a 24-bit number to 3 big-endian bytes. */
    static void bigEndian24BitToChars(int32_t value, void* destBytes) noexcept;

    //==============================================================================
    /** Constructs a 16-bit integer from its constituent bytes, in order of significance. */
    constexpr static uint16_t makeInt(uint8_t leastSig, uint8_t mostSig) noexcept;

    /** Constructs a 32-bit integer from its constituent bytes, in order of significance. */
    constexpr static uint32_t makeInt(uint8_t leastSig, uint8_t byte1, uint8_t byte2,
                                      uint8_t mostSig) noexcept;

    /** Constructs a 64-bit integer from its constituent bytes, in order of significance. */
    constexpr static uint64_t makeInt(uint8_t leastSig, uint8_t byte1, uint8_t byte2, uint8_t byte3,
                                      uint8_t byte4, uint8_t byte5, uint8_t byte6,
                                      uint8_t mostSig) noexcept;

    //==============================================================================
    /** Returns true if the current CPU is big-endian. */
    constexpr static bool isBigEndian() noexcept
    {
#if defined(GLUE_LITTLE_ENDIAN)
        return false;
#else
        return true;
#endif
    }

private:
    ByteOrder() = delete;
};

//==============================================================================
constexpr inline uint16_t ByteOrder::swap(uint16_t v) noexcept
{
    return static_cast<uint16_t>((v << 8) | (v >> 8));
}
constexpr inline int16_t ByteOrder::swap(int16_t v) noexcept
{
    return static_cast<int16_t>(swap(static_cast<uint16_t>(v)));
}
inline int32_t ByteOrder::swap(int32_t v) noexcept
{
    return static_cast<int32_t>(swap(static_cast<uint32_t>(v)));
}
inline int64_t ByteOrder::swap(int64_t v) noexcept
{
    return static_cast<int64_t>(swap(static_cast<uint64_t>(v)));
}
inline float ByteOrder::swap(float v) noexcept
{
    union
    {
        uint32_t asUInt;
        float asFloat;
    } n;
    n.asFloat = v;
    n.asUInt = swap(n.asUInt);
    return n.asFloat;
}
inline double ByteOrder::swap(double v) noexcept
{
    union
    {
        uint64_t asUInt;
        double asFloat;
    } n;
    n.asFloat = v;
    n.asUInt = swap(n.asUInt);
    return n.asFloat;
}

#if defined(GLUE_COMPILER_MSVC) && !defined(__INTEL_COMPILER)
    #pragma intrinsic(_byteswap_ulong)
#endif

inline uint32_t ByteOrder::swap(uint32_t n) noexcept
{
#if defined(GLUE_ARCH_X86) && defined(GLUE_USE_GCC_INLINE_ASM)
    asm("bswap %%eax" : "=a"(n) : "a"(n));
    return n;
#elif defined(GLUE_COMPILER_MSVC)
    return _byteswap_ulong(n);
#elif defined(GLUE_OS_ANDROID)
    return bswap_32(n);
#else
    return (n << 24) | (n >> 24) | ((n & 0xff00) << 8) | ((n & 0xff0000) >> 8);
#endif
}

inline uint64_t ByteOrder::swap(uint64_t value) noexcept
{
#if defined(GLUE_COMPILER_MSVC)
    return _byteswap_uint64(value);
#else
    return (((uint64_t) swap((uint32_t) value)) << 32) | swap((uint32_t) (value >> 32));
#endif
}

constexpr inline uint16_t ByteOrder::makeInt(uint8_t b0, uint8_t b1) noexcept
{
    return static_cast<uint16_t>(static_cast<uint16_t>(b0) | (static_cast<uint16_t>(b1) << 8));
}

constexpr inline uint32_t ByteOrder::makeInt(uint8_t b0, uint8_t b1, uint8_t b2,
                                             uint8_t b3) noexcept
{
    return static_cast<uint32_t>(b0) | (static_cast<uint32_t>(b1) << 8) |
           (static_cast<uint32_t>(b2) << 16) | (static_cast<uint32_t>(b3) << 24);
}

constexpr inline uint64_t ByteOrder::makeInt(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3,
                                             uint8_t b4, uint8_t b5, uint8_t b6,
                                             uint8_t b7) noexcept
{
    return static_cast<uint64_t>(b0) | (static_cast<uint64_t>(b1) << 8) |
           (static_cast<uint64_t>(b2) << 16) | (static_cast<uint64_t>(b3) << 24) |
           (static_cast<uint64_t>(b4) << 32) | (static_cast<uint64_t>(b5) << 40) |
           (static_cast<uint64_t>(b6) << 48) | (static_cast<uint64_t>(b7) << 56);
}

constexpr inline uint16_t ByteOrder::littleEndianShort(const void* bytes) noexcept
{
    return makeInt(static_cast<const uint8_t*>(bytes)[0], static_cast<const uint8_t*>(bytes)[1]);
}
constexpr inline uint32_t ByteOrder::littleEndianInt(const void* bytes) noexcept
{
    return makeInt(static_cast<const uint8_t*>(bytes)[0], static_cast<const uint8_t*>(bytes)[1],
                   static_cast<const uint8_t*>(bytes)[2], static_cast<const uint8_t*>(bytes)[3]);
}
constexpr inline uint64_t ByteOrder::littleEndianInt64(const void* bytes) noexcept
{
    return makeInt(static_cast<const uint8_t*>(bytes)[0], static_cast<const uint8_t*>(bytes)[1],
                   static_cast<const uint8_t*>(bytes)[2], static_cast<const uint8_t*>(bytes)[3],
                   static_cast<const uint8_t*>(bytes)[4], static_cast<const uint8_t*>(bytes)[5],
                   static_cast<const uint8_t*>(bytes)[6], static_cast<const uint8_t*>(bytes)[7]);
}

constexpr inline uint16_t ByteOrder::bigEndianShort(const void* bytes) noexcept
{
    return makeInt(static_cast<const uint8_t*>(bytes)[1], static_cast<const uint8_t*>(bytes)[0]);
}
constexpr inline uint32_t ByteOrder::bigEndianInt(const void* bytes) noexcept
{
    return makeInt(static_cast<const uint8_t*>(bytes)[3], static_cast<const uint8_t*>(bytes)[2],
                   static_cast<const uint8_t*>(bytes)[1], static_cast<const uint8_t*>(bytes)[0]);
}
constexpr inline uint64_t ByteOrder::bigEndianInt64(const void* bytes) noexcept
{
    return makeInt(static_cast<const uint8_t*>(bytes)[7], static_cast<const uint8_t*>(bytes)[6],
                   static_cast<const uint8_t*>(bytes)[5], static_cast<const uint8_t*>(bytes)[4],
                   static_cast<const uint8_t*>(bytes)[3], static_cast<const uint8_t*>(bytes)[2],
                   static_cast<const uint8_t*>(bytes)[1], static_cast<const uint8_t*>(bytes)[0]);
}

constexpr inline int32_t ByteOrder::littleEndian24Bit(const void* bytes) noexcept
{
    return (int32_t) ((((uint32_t) static_cast<const int8_t*>(bytes)[2]) << 16) |
                      (((uint32_t) static_cast<const uint8_t*>(bytes)[1]) << 8) |
                      ((uint32_t) static_cast<const uint8_t*>(bytes)[0]));
}
constexpr inline int32_t ByteOrder::bigEndian24Bit(const void* bytes) noexcept
{
    return (int32_t) ((((uint32_t) static_cast<const int8_t*>(bytes)[0]) << 16) |
                      (((uint32_t) static_cast<const uint8_t*>(bytes)[1]) << 8) |
                      ((uint32_t) static_cast<const uint8_t*>(bytes)[2]));
}

inline void ByteOrder::littleEndian24BitToChars(int32_t value, void* destBytes) noexcept
{
    static_cast<uint8_t*>(destBytes)[0] = (uint8_t) value;
    static_cast<uint8_t*>(destBytes)[1] = (uint8_t) (value >> 8);
    static_cast<uint8_t*>(destBytes)[2] = (uint8_t) (value >> 16);
}
inline void ByteOrder::bigEndian24BitToChars(int32_t value, void* destBytes) noexcept
{
    static_cast<uint8_t*>(destBytes)[0] = (uint8_t) (value >> 16);
    static_cast<uint8_t*>(destBytes)[1] = (uint8_t) (value >> 8);
    static_cast<uint8_t*>(destBytes)[2] = (uint8_t) value;
}

GLUE_END_NAMESPACE
