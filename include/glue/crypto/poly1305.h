//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/common.hpp"

#include <vector>

#include "glue/crypto/key_spec.h"

namespace glue {

/**
 * DJB's Poly1305
 * Important note: each key can only be used once
 */
class GLUE_API Poly1305 final
{
public:
    void clear();

    void set_key(const uint8_t key[], size_t key_len);

    void update(const uint8_t[], size_t);

    void final(uint8_t[]);

    size_t output_length() const
    {
        return 16;
    }

    /**
     * @return minimum allowed key length
     */
    size_t maximum_keylength() const
    {
        return m_key_spec.maximum_keylength();
    }

    /**
     * @return maximum allowed key length
     */
    size_t minimum_keylength() const
    {
        return m_key_spec.minimum_keylength();
    }

    /**
     * Check whether a given key length is valid for this algorithm.
     * @param length the key length to be checked.
     * @return true if the key length is valid.
     */
    bool valid_keylength(size_t length) const
    {
        return m_key_spec.valid_keylength(length);
    }

private:
    void key_schedule(const uint8_t[], size_t);

    void verify_key_set(bool cond) const;

    KeySpec m_key_spec{32};

    std::vector<uint64_t> m_poly;
    std::vector<uint8_t> m_buf;
    size_t m_buf_pos = 0;
};

} // namespace glue
