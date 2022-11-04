//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/crypto/mem_ops.h"

#include <algorithm>
#include <deque>
#include <type_traits>
#include <vector>

namespace glue {
namespace crypto {

template <typename T, typename Alloc>
size_t buffer_insert(std::vector<T, Alloc> &buf, size_t buf_offset,
                     const T input[], size_t input_length) {
    // OCL_ASSERT_NOMSG(buf_offset <= buf.size());
    const size_t to_copy = std::min(input_length, buf.size() - buf_offset);
    if (to_copy > 0) {
        copy_mem(&buf[buf_offset], input, to_copy);
    }
    return to_copy;
}

template <typename T, typename Alloc, typename Alloc2>
size_t buffer_insert(std::vector<T, Alloc> &buf, size_t buf_offset,
                     const std::vector<T, Alloc2> &input) {
    // OCL_ASSERT_NOMSG(buf_offset <= buf.size());
    const size_t to_copy = std::min(input.size(), buf.size() - buf_offset);
    if (to_copy > 0) {
        copy_mem(&buf[buf_offset], input.data(), to_copy);
    }
    return to_copy;
}

/**
 * Zeroise the values; length remains unchanged
 * @param vec the vector to zeroise
 */
template <typename T, typename Alloc> void zeroise(std::vector<T, Alloc> &vec) {
    clear_mem(vec.data(), vec.size());
}

/**
 * Zeroise the values then free the memory
 * @param vec the vector to zeroise and free
 */
template <typename T, typename Alloc> void zap(std::vector<T, Alloc> &vec) {
    zeroise(vec);
    vec.clear();
    vec.shrink_to_fit();
}

} // namespace crypto
GLUE_END_NAMESPACE
