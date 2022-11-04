//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

namespace glue {
namespace crypto {

/**
 * Represents the length requirements on an algorithm key
 */
class KeySpec final {
  public:
    /**
     * Constructor for fixed length keys
     * @param keylen the supported key length
     */
    explicit KeySpec(size_t keylen)
        : m_min_keylen(keylen), m_max_keylen(keylen), m_keylen_mod(1) {}

    /**
     * Constructor for variable length keys
     * @param min_k the smallest supported key length
     * @param max_k the largest supported key length
     * @param k_mod the number of bytes the key must be a multiple of
     */
    KeySpec(size_t min_k, size_t max_k, size_t k_mod = 1)
        : m_min_keylen(min_k), m_max_keylen(max_k ? max_k : min_k),
          m_keylen_mod(k_mod) {}

    /**
     * @param length is a key length in bytes
     * @return true iff this length is a valid length for this algo
     */
    bool valid_keylength(size_t length) const {
        return ((length >= m_min_keylen) && (length <= m_max_keylen) &&
                (length % m_keylen_mod == 0));
    }

    /**
     * @return minimum key length in bytes
     */
    size_t minimum_keylength() const { return m_min_keylen; }

    /**
     * @return maximum key length in bytes
     */
    size_t maximum_keylength() const { return m_max_keylen; }

    /**
     * @return key length multiple in bytes
     */
    size_t keylength_multiple() const { return m_keylen_mod; }

    /*
     * Multiplies all length requirements with the given factor
     * @param n the multiplication factor
     * @return a key length specification multiplied by the factor
     */
    KeySpec multiple(size_t n) const {
        return KeySpec(n * m_min_keylen, n * m_max_keylen, n * m_keylen_mod);
    }

  private:
    size_t m_min_keylen, m_max_keylen, m_keylen_mod;
};

} // namespace crypto
GLUE_END_NAMESPACE
