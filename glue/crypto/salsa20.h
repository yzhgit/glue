//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include <cassert>
#include <climits>
#include <cstdint>
#include <cstring>

namespace glue {
namespace crypto {

/**
 * Represents Salsa20 cypher. Supports only 256-bit keys.
 */
class Salsa20 {
  public:
    /// Helper constants
    enum : uint8_t {
        VECTOR_SIZE = 16,
        BLOCK_SIZE = 64,
        KEY_SIZE = 32,
        IV_SIZE = 8
    };

    /**
     * \brief Constructs cypher with given key.
     * \param[in] key 256-bit key
     */
    inline Salsa20(const uint8_t *key = nullptr);
    Salsa20(const Salsa20 &) = default;
    Salsa20(Salsa20 &&) = default;
    ~Salsa20() = default;
    Salsa20 &operator=(const Salsa20 &) = default;
    Salsa20 &operator=(Salsa20 &&) = default;

    /**
     * \brief Sets key.
     * \param[in] key 256-bit key
     */
    inline void setKey(const uint8_t *key);

    /**
     * \brief Sets IV.
     * \param[in] iv 64-bit IV
     */
    inline void setIv(const uint8_t *iv);

    /**
     * \brief Generates key stream.
     * \param[out] output generated key stream
     */
    inline void generateKeyStream(uint8_t output[BLOCK_SIZE]);

    /**
     * \brief Processes blocks.
     * \param[in] input input
     * \param[out] output output
     * \param[in] numBlocks number of blocks
     */
    inline void processBlocks(const uint8_t *input, uint8_t *output,
                              size_t numBlocks);

    /**
     * \brief Processes bytes.
     *
     * This function should be used carefully. If number of bytes is not
     * multiple of block size, then next call to the processBlocks function will
     * be invalid. Normally this function should be used once at the end of
     * encryption or decryption. \param[in] input input \param[out] output
     * output \param[in] numBytes number of bytes
     */
    inline void processBytes(const uint8_t *input, uint8_t *output,
                             size_t numBytes);

  private:
    /**
     * \brief Rotates value.
     * \param[in] value value
     * \param[in] numBits number of bits to rotate
     * \return result of the rotation
     */
    inline uint32_t rotate(uint32_t value, uint32_t numBits);

    /**
     * \brief Converts 32-bit unsigned integer value to the array of bytes.
     * \param[in] value 32-bit unsigned integer value
     * \param[out] array array of bytes
     */
    inline void convert(uint32_t value, uint8_t *array);

    /**
     * \brief Converts array of bytes to the 32-bit unsigned integer value.
     * \param[in] array array of bytes
     * \return 32-bit unsigned integer value
     */
    inline uint32_t convert(const uint8_t *array);

    // Data members
    uint32_t vector_[VECTOR_SIZE];
};

} // namespace crypto
} // namespace glue
