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
 * DJB's Salsa20 (and XSalsa20)
 */
class Salsa20 final
{
public:
    Salsa20() = default;

    void clear();

    void set_key(const uint8_t key[], size_t key_len);

    void set_iv(const uint8_t iv[], size_t iv_len);

    void cipher(const uint8_t in[], uint8_t out[], size_t length);

    void seek(uint64_t offset);

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

    bool valid_iv_length(size_t iv_len) const;

    size_t default_iv_length() const;

    static void salsa_core(uint8_t output[64], const uint32_t input[16], size_t rounds);

private:
    void key_schedule(const uint8_t key[], size_t key_len);

    void initialize_state();

    void verify_key_set(bool cond) const;

    KeySpec m_key_spec{16, 32, 16};

    std::vector<uint32_t> m_key;
    std::vector<uint32_t> m_state;
    std::vector<uint8_t> m_buffer;
    size_t m_position = 0;
};

} // namespace glue
