//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/common.h"

#include <vector>

namespace glue {

class GLUE_API SHA_512
{
public:
    /// split into 128 byte blocks (=> 1024 bits), hash is 64 bytes long
    enum
    {
        BlockSize = 128
    };
    enum
    {
        DigestSize = 64
    };

    SHA_512();

    void clear();
    void update(const uint8_t input[], size_t length);
    // output is DigestSize long
    void final(uint8_t output[]);

private:
    void compress_n(const uint8_t input[], size_t blocks);

private:
    /// size of processed data in bytes
    uint64_t m_count;
    /// valid bytes in m_buffer
    size_t m_position;
    /// bytes not processed yet
    std::vector<uint8_t> m_buffer;
    /// digest value
    std::vector<uint64_t> m_digest;
};

} // namespace glue
