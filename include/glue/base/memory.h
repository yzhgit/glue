//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/common.h"

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

//==============================================================================
/* In a Windows DLL build, we'll expose some malloc/free functions that live inside the DLL, and use
   these for allocating all the objects - that way all glue objects in the DLL and in the host will
   live in the same heap, avoiding problems when an object is created in one module and passed
   across to another where it is deleted. By piggy-backing on the GLUE_LEAK_DETECTOR macro, these
   allocators can be injected into most glue classes.
*/
#if defined(GLUE_COMPILER_MSVC) && defined(GLUE_DLL) && !(GLUE_DISABLE_DLL_ALLOCATORS || DOXYGEN)

GLUE_API void* glueDLL_malloc(size_t);
GLUE_API void glueDLL_free(void*);

    #define GLUE_LEAK_DETECTOR(OwnerClass)                                                         \
    public:                                                                                        \
        static void* operator new(size_t sz)                                                       \
        {                                                                                          \
            return glue::glueDLL_malloc(sz);                                                       \
        }                                                                                          \
        static void* operator new(size_t, void* p)                                                 \
        {                                                                                          \
            return p;                                                                              \
        }                                                                                          \
        static void operator delete(void* p)                                                       \
        {                                                                                          \
            glue::glueDLL_free(p);                                                                 \
        }                                                                                          \
        static void operator delete(void*, void*)                                                  \
        {}
#endif

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

} // namespace glue
