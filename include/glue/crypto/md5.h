//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include <stdint.h>
#include <vector>

namespace glue {
namespace crypto {

class MD5 final {
  public:
    enum { BlockSize = 64 };
    enum { DigestSize = 16 };

    MD5();

    void clear();
    void update(const uint8_t input[], size_t length);
    // output is DigestSize long
    void final(uint8_t output[]);

  private:
    void compress_n(const uint8_t[], size_t blocks);

  private:
    /// size of processed data in bytes
    uint64_t m_count;
    /// valid bytes in m_buffer
    size_t m_position;
    /// bytes not processed yet
    std::vector<uint8_t> m_buffer;
    /// message buffer
    std::vector<uint32_t> m_M;
    /// digest value
    std::vector<uint32_t> m_digest;
};

} // namespace crypto
GLUE_END_NAMESPACE
