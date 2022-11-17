//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "mem_ops.h"
#include "ct_utils.h"

namespace glue {

void secure_scrub_memory(void* ptr, size_t n)
{
#if defined(GLUE_OS_WINDOWS)
    ::RtlSecureZeroMemory(ptr, n);

#elif defined(GLUE_OS_BSD)
    ::explicit_bzero(ptr, n);

#else
    /*
    Call memset through a static volatile pointer, which the compiler
    should not elide. This construct should be safe in conforming
    compilers, but who knows. I did confirm that on x86-64 GCC 6.1 and
    Clang 3.8 both create code that saves the memset address in the
    data segment and unconditionally loads and jumps to that address.
    */
    static void* (*const volatile memset_ptr)(void*, int, size_t) = std::memset;
    (memset_ptr)(ptr, 0, n);

#endif
}

uint8_t ct_compare_u8(const uint8_t x[], const uint8_t y[], size_t len)
{
    volatile uint8_t difference = 0;

    for (size_t i = 0; i != len; ++i) difference = difference | (x[i] ^ y[i]);

    return CT::Mask<uint8_t>::is_zero(difference).value();
}

} // namespace glue
