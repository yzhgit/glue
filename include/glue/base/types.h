//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include <cstddef> // std::ptrdiff_t, std::size_t
#include <cstdint> // std::int8_t, std::int16_t, etc

namespace glue {

//-------------------------------------------------------------------------

using uchar = unsigned char;   ///< Unsigned char type
using ushort = unsigned short; ///< Unsigned short type
using uint = unsigned int;     ///< Unsigned int type
using ulong = unsigned long;   ///< Unsigned long type

//-------------------------------------------------------------------------

using size_t = std::size_t;     ///< Type representing size of objects
using align_t = std::size_t;    ///< Type representing alignment of an object
using index_t = std::ptrdiff_t; ///< Type representing index of objects

//------------------------------------------------------------------------
// Integer Types
//------------------------------------------------------------------------

// Basic signed
using int8 = std::int8_t;   ///< Signed 8-bit integer
using int16 = std::int16_t; ///< Signed 16-bit integer
using int32 = std::int32_t; ///< Signed 32-bit integer
using int64 = std::int64_t; ///< Signed 64-bit integer

// Basic unsigned
using uint8 = std::uint8_t;   ///< Unsigned 8-bit integer
using uint16 = std::uint16_t; ///< Unsigned 16-bit integer
using uint32 = std::uint32_t; ///< Unsigned 32-bit integer
using uint64 = std::uint64_t; ///< Unsigned 64-bit integer

//! 8 bit unsigned variable - byte.
using byte = uint8;

//------------------------------------------------------------------------
// Floating point types
//
// It is worth noting that float and double may not always be
// IEEE-754 floats/doubles in which they are 32/64 bits respectively.
//
// However, there is rarely a case in which float will be practically less
// than 32 bits, and having it larger than 32 bits should not pose a
// problem.
//------------------------------------------------------------------------

using float32 = float;  ///< 32-bit floating point (single precision)
using float64 = double; ///< 64-bit floating point (double precision)

// Static check of Integral types
// ...otherwise things might fail with compilers!
static_assert(1 == sizeof(byte), "size of byte is not 1 byte!");
static_assert(1 == sizeof(uint8), "size of uint8 is not 1 byte!");
static_assert(1 == sizeof(int8), "size of int8 is not 1 byte!");
static_assert(2 == sizeof(int16), "size of int16 is not 2 bytes!");
static_assert(2 == sizeof(uint16), "size of uint16 is not 2 bytes!");
static_assert(4 == sizeof(int32), "size of int32 is not 4 bytes!");
static_assert(4 == sizeof(uint32), "size of uint32 is not 4 bytes!");
static_assert(8 == sizeof(int64), "size of uint64 is not 8 bytes!");
static_assert(8 == sizeof(uint64), "size of uint64 is not 8 bytes!");

static_assert(4 == sizeof(float32), "size of float32 is not 4 bytes!");
static_assert(8 == sizeof(float64), "size of float64 is not 8 bytes!");

} // namespace glue
