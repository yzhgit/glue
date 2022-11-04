//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include <stdint.h>
#include <vector>

namespace glue {
namespace crypto {

class SHA_256 {
  public:
    /// split into 128 byte blocks (=> 1024 bits), hash is 64 bytes long
    enum { BlockSize = 64 };
    enum { DigestSize = 32 };

    SHA_256();

    void clear();
    void update(const uint8_t input[], size_t length);
    // output is DigestSize long
    void final(uint8_t output[]);

  private:
    void compress_n(const uint8_t input[], size_t blocks);

    /*
     * Perform a SHA-256 compression. For internal use
     */
    static void compress_digest(std::vector<uint32_t> &digest,
                                const uint8_t input[], size_t blocks);

  private:
#if defined(GL_HAS_SHA2_32_ARMV8)
    static void compress_digest_armv8(secure_vector<uint32_t> &digest,
                                      const uint8_t input[], size_t blocks);
#endif

#if defined(GL_HAS_SHA2_32_X86)
    static void compress_digest_x86(secure_vector<uint32_t> &digest,
                                    const uint8_t input[], size_t blocks);
#endif

  private:
    /// size of processed data in bytes
    uint64_t m_count;
    /// valid bytes in m_buffer
    size_t m_position;
    /// bytes not processed yet
    std::vector<uint8_t> m_buffer;
    /// digest value
    std::vector<uint32_t> m_digest;
};

} // namespace crypto
GLUE_END_NAMESPACE
