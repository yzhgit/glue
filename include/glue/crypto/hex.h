//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/common.h"

#include <string>
#include <vector>

namespace glue {

/**
 * Perform hex encoding
 * @param output an array of at least input_length*2 bytes
 * @param input is some binary data
 * @param input_length length of input in bytes
 * @param uppercase should output be upper or lower case?
 */
GLUE_API void hex_encode(char output[], const uint8_t input[], size_t input_length,
                         bool uppercase = true);

/**
 * Perform hex encoding
 * @param input some input
 * @param input_length length of input in bytes
 * @param uppercase should output be upper or lower case?
 * @return hexadecimal representation of input
 */
GLUE_API std::string hex_encode(const uint8_t input[], size_t input_length, bool uppercase = true);

/**
 * Perform hex encoding
 * @param input some input
 * @param uppercase should output be upper or lower case?
 * @return hexadecimal representation of input
 */
template <typename Alloc>
std::string hex_encode(const std::vector<uint8_t, Alloc>& input, bool uppercase = true)
{
    return hex_encode(input.data(), input.size(), uppercase);
}

/**
* Perform hex decoding
* @param output an array of at least input_length/2 bytes
* @param input some hex input
* @param input_length length of input in bytes
* @param input_consumed is an output parameter which says how many
*        bytes of input were actually consumed. If less than
*        input_length, then the range input[consumed:length]
*        should be passed in later along with more input.
* @param ignore_ws ignore whitespace on input; if false, throw an
                exception if whitespace is encountered
* @return number of bytes written to output
*/
GLUE_API size_t hex_decode(uint8_t output[], const char input[], size_t input_length,
                           size_t& input_consumed, bool ignore_ws = true);

/**
* Perform hex decoding
* @param output an array of at least input_length/2 bytes
* @param input some hex input
* @param input_length length of input in bytes
* @param ignore_ws ignore whitespace on input; if false, throw an
                exception if whitespace is encountered
* @return number of bytes written to output
*/
GLUE_API size_t hex_decode(uint8_t output[], const char input[], size_t input_length,
                           bool ignore_ws = true);

/**
* Perform hex decoding
* @param output an array of at least input_length/2 bytes
* @param input some hex input
* @param ignore_ws ignore whitespace on input; if false, throw an
                exception if whitespace is encountered
* @return number of bytes written to output
*/
GLUE_API size_t hex_decode(uint8_t output[], const std::string& input, bool ignore_ws = true);

/**
* Perform hex decoding
* @param input some hex input
* @param input_length the length of input in bytes
* @param ignore_ws ignore whitespace on input; if false, throw an
                exception if whitespace is encountered
* @return decoded hex output
*/
GLUE_API std::vector<uint8_t> hex_decode(const char input[], size_t input_length,
                                         bool ignore_ws = true);

/**
* Perform hex decoding
* @param input some hex input
* @param ignore_ws ignore whitespace on input; if false, throw an
                exception if whitespace is encountered
* @return decoded hex output
*/
GLUE_API std::vector<uint8_t> hex_decode(const std::string& input, bool ignore_ws = true);

} // namespace glue
