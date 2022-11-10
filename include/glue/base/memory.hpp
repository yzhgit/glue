//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/common.hpp"

#include <memory>
#include <type_traits>

namespace glue {

//==============================================================================
/** Fills a block of memory with zeros. */
inline void zeromem(void* memory, size_t numBytes) noexcept
{
    memset(memory, 0, numBytes);
}

/** Overwrites a structure or object with zeros. */
template <typename Type>
inline void zerostruct(Type& structure) noexcept
{
    memset((void*) &structure, 0, sizeof(structure));
}

/** Delete an object pointer, and sets the pointer to null.

    Remember that it's not good c++ practice to use delete directly - always try to use a
   std::unique_ptr or other automatic lifetime-management system rather than resorting to deleting
   raw pointers!
*/
template <typename Type>
inline void deleteAndZero(Type& pointer)
{
    delete pointer;
    pointer = nullptr;
}

/** A handy function to round up a pointer to the nearest multiple of a given number of bytes.
    alignmentBytes must be a power of two. */
template <typename Type, typename IntegerType>
inline Type* snapPointerToAlignment(Type* basePointer, IntegerType alignmentBytes) noexcept
{
    return (Type*) ((((size_t) basePointer) + (alignmentBytes - 1)) & ~(alignmentBytes - 1));
}

/** A handy function which returns the difference between any two pointers, in bytes.
    The address of the second pointer is subtracted from the first, and the difference in bytes is
   returned.
*/
template <typename Type1, typename Type2>
inline int getAddressDifference(Type1* pointer1, Type2* pointer2) noexcept
{
    return (int) (((const char*) pointer1) - (const char*) pointer2);
}

/** If a pointer is non-null, this returns a new copy of the object that it points to, or safely
   returns nullptr if the pointer is null.
*/
template <class Type>
inline Type* createCopyIfNotNull(const Type* objectToCopy)
{
    return objectToCopy != nullptr ? new Type(*objectToCopy) : nullptr;
}

//==============================================================================
/** A handy function to read un-aligned memory without a performance penalty or bus-error. */
template <typename Type>
inline Type readUnaligned(const void* srcPtr) noexcept
{
    Type value;
    memcpy(&value, srcPtr, sizeof(Type));
    return value;
}

/** A handy function to write un-aligned memory without a performance penalty or bus-error. */
template <typename Type>
inline void writeUnaligned(void* dstPtr, Type value) noexcept
{
    memcpy(dstPtr, &value, sizeof(Type));
}

//==============================================================================
/** Casts a pointer to another type via `void*`, which suppresses the cast-align
    warning which sometimes arises when casting pointers to types with different
    alignment.
    You should only use this when you know for a fact that the input pointer points
    to a region that has suitable alignment for `Type`, e.g. regions returned from
    malloc/calloc that should be suitable for any non-over-aligned type.
*/
template <typename Type, typename std::enable_if<std::is_pointer<Type>::value, int>::type = 0>
inline Type unalignedPointerCast(void* ptr) noexcept
{
    return reinterpret_cast<Type>(ptr);
}

/** Casts a pointer to another type via `void*`, which suppresses the cast-align
    warning which sometimes arises when casting pointers to types with different
    alignment.
    You should only use this when you know for a fact that the input pointer points
    to a region that has suitable alignment for `Type`, e.g. regions returned from
    malloc/calloc that should be suitable for any non-over-aligned type.
*/
template <typename Type, typename std::enable_if<std::is_pointer<Type>::value, int>::type = 0>
inline Type unalignedPointerCast(const void* ptr) noexcept
{
    return reinterpret_cast<Type>(ptr);
}

/** A handy function which adds a number of bytes to any type of pointer and returns the result.
    This can be useful to avoid casting pointers to a char* and back when you want to move them by
    a specific number of bytes,
*/
template <typename Type, typename IntegerType>
inline Type* addBytesToPointer(Type* basePointer, IntegerType bytes) noexcept
{
    return unalignedPointerCast<Type*>(reinterpret_cast<char*>(basePointer) + bytes);
}

/** A handy function which adds a number of bytes to any type of pointer and returns the result.
    This can be useful to avoid casting pointers to a char* and back when you want to move them by
    a specific number of bytes,
*/
template <typename Type, typename IntegerType>
inline const Type* addBytesToPointer(const Type* basePointer, IntegerType bytes) noexcept
{
    return unalignedPointerCast<const Type*>(reinterpret_cast<const char*>(basePointer) + bytes);
}

/** Converts an owning raw pointer into a unique_ptr, deriving the
    type of the unique_ptr automatically.

    This should only be used with pointers to single objects.
    Do NOT pass a pointer to an array to this function, as the
    destructor of the unique_ptr will incorrectly call `delete`
    instead of `delete[]` on the pointer.
*/
template <typename T>
std::unique_ptr<T> rawToUniquePtr(T* ptr)
{
    return std::unique_ptr<T>(ptr);
}

namespace memory_internal {

    // Traits to select proper overload and return type for `glue::make_unique<>`.
    template <typename T>
    struct MakeUniqueResult
    {
        using scalar = std::unique_ptr<T>;
    };
    template <typename T>
    struct MakeUniqueResult<T[]>
    {
        using array = std::unique_ptr<T[]>;
    };
    template <typename T, size_t N>
    struct MakeUniqueResult<T[N]>
    {
        using invalid = void;
    };

} // namespace memory_internal

// gcc 4.8 has __cplusplus at 201301 but the libstdc++ shipped with it doesn't
// define make_unique.  Other supported compilers either just define __cplusplus
// as 201103 but have make_unique (msvc), or have make_unique whenever
// __cplusplus > 201103 (clang).
#if (__cplusplus > 201103L || defined(_MSC_VER)) &&                                                \
    !(defined(__GLIBCXX__) && !defined(__cpp_lib_make_unique))
using std::make_unique;
#else
// -----------------------------------------------------------------------------
// Function Template: make_unique<T>()
// -----------------------------------------------------------------------------
//
// Creates a `std::unique_ptr<>`, while avoiding issues creating temporaries
// during the construction process. `glue::make_unique<>` also avoids redundant
// type declarations, by avoiding the need to explicitly use the `new` operator.
//
// This implementation of `glue::make_unique<>` is designed for C++11 code and
// will be replaced in C++14 by the equivalent `std::make_unique<>` abstraction.
// `glue::make_unique<>` is designed to be 100% compatible with
// `std::make_unique<>` so that the eventual migration will involve a simple
// rename operation.
//
// For more background on why `std::unique_ptr<T>(new T(a,b))` is problematic,
// see Herb Sutter's explanation on
// (Exception-Safe Function Calls)[https://herbsutter.com/gotw/_102/].
// (In general, reviewers should treat `new T(a,b)` with scrutiny.)
//
// Example usage:
//
//    auto p = make_unique<X>(args...);  // 'p'  is a std::unique_ptr<X>
//    auto pa = make_unique<X[]>(5);     // 'pa' is a std::unique_ptr<X[]>
//
// Three overloads of `glue::make_unique` are required:
//
//   - For non-array T:
//
//       Allocates a T with `new T(std::forward<Args> args...)`,
//       forwarding all `args` to T's constructor.
//       Returns a `std::unique_ptr<T>` owning that object.
//
//   - For an array of unknown bounds T[]:
//
//       `glue::make_unique<>` will allocate an array T of type U[] with
//       `new U[n]()` and return a `std::unique_ptr<U[]>` owning that array.
//
//       Note that 'U[n]()' is different from 'U[n]', and elements will be
//       value-initialized. Note as well that `std::unique_ptr` will perform its
//       own destruction of the array elements upon leaving scope, even though
//       the array [] does not have a default destructor.
//
//       NOTE: an array of unknown bounds T[] may still be (and often will be)
//       initialized to have a size, and will still use this overload. E.g:
//
//         auto my_array = glue::make_unique<int[]>(10);
//
//   - For an array of known bounds T[N]:
//
//       `glue::make_unique<>` is deleted (like with `std::make_unique<>`) as
//       this overload is not useful.
//
//       NOTE: an array of known bounds T[N] is not considered a useful
//       construction, and may cause undefined behavior in templates. E.g:
//
//         auto my_array = glue::make_unique<int[10]>();
//
//       In those cases, of course, you can still use the overload above and
//       simply initialize it to its desired size:
//
//         auto my_array = glue::make_unique<int[]>(10);

// `glue::make_unique` overload for non-array types.
template <typename T, typename... Args>
typename memory_internal::MakeUniqueResult<T>::scalar make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// `glue::make_unique` overload for an array T[] of unknown bounds.
// The array allocation needs to use the `new T[size]` form and cannot take
// element constructor arguments. The `std::unique_ptr` will manage destructing
// these array elements.
template <typename T>
typename memory_internal::MakeUniqueResult<T>::array make_unique(size_t n)
{
    return std::unique_ptr<T>(new typename std::remove_extent<T>::type[n]());
}

// `glue::make_unique` overload for an array T[N] of known bounds.
// This construction will be rejected.
template <typename T, typename... Args>
typename memory_internal::MakeUniqueResult<T>::invalid make_unique(Args&&... /* args */) = delete;
#endif

} // namespace glue
