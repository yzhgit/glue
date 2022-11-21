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
* Perform base64 encoding
* @param output an array of at least base64_encode_max_output bytes
* @param input is some binary data
* @param input_length length of input in bytes
* @param input_consumed is an output parameter which says how many
*        bytes of input were actually consumed. If less than
*        input_length, then the range input[consumed:length]
*        should be passed in later along with more input.
* @param final_inputs true iff this is the last input, in which case
         padding chars will be applied if needed
* @return number of bytes written to output
*/
GLUE_API size_t base64_encode(char output[], const uint8_t input[], size_t input_length,
                              size_t& input_consumed, bool final_inputs);

/**
 * Perform base64 encoding
 * @param input some input
 * @param input_length length of input in bytes
 * @return base64adecimal representation of input
 */
GLUE_API std::string base64_encode(const uint8_t input[], size_t input_length);

/**
 * Perform base64 encoding
 * @param input some input
 * @return base64adecimal representation of input
 */
inline std::string base64_encode(const std::string& input)
{
    return base64_encode((const uint8_t*) input.data(), input.size());
}

/**
* Perform base64 decoding
* @param output an array of at least base64_decode_max_output bytes
* @param input some base64 input
* @param input_length length of input in bytes
* @param input_consumed is an output parameter which says how many
*        bytes of input were actually consumed. If less than
*        input_length, then the range input[consumed:length]
*        should be passed in later along with more input.
* @param final_inputs true iff this is the last input, in which case
         padding is allowed
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return number of bytes written to output
*/
GLUE_API size_t base64_decode(uint8_t output[], const char input[], size_t input_length,
                              size_t& input_consumed, bool final_inputs, bool ignore_ws = true);

/**
* Perform base64 decoding
* @param output an array of at least base64_decode_max_output bytes
* @param input some base64 input
* @param input_length length of input in bytes
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return number of bytes written to output
*/
GLUE_API size_t base64_decode(uint8_t output[], const char input[], size_t input_length,
                              bool ignore_ws = true);

/**
* Perform base64 decoding
* @param output an array of at least base64_decode_max_output bytes
* @param input some base64 input
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return number of bytes written to output
*/
inline size_t base64_decode(uint8_t output[], const std::string& input, bool ignore_ws = true)
{
    return base64_decode(output, input.data(), input.size(), ignore_ws);
}

/**
* Perform base64 decoding
* @param input some base64 input
* @param input_length the length of input in bytes
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return decoded base64 output
*/
GLUE_API std::string base64_decode(const char input[], size_t input_length, bool ignore_ws = true);

/**
* Perform base64 decoding
* @param input some base64 input
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return decoded base64 output
*/
inline std::string base64_decode(const std::string& input, bool ignore_ws = true)
{
    return base64_decode(input.data(), input.size(), ignore_ws);
}

/**
 * Calculate the size of output buffer for base64_encode
 * @param input_length the length of input in bytes
 * @return the size of output buffer in bytes
 */
GLUE_API size_t base64_encode_max_output(size_t input_length);

/**
 * Calculate the size of output buffer for base64_decode
 * @param input_length the length of input in bytes
 * @return the size of output buffer in bytes
 */
GLUE_API size_t base64_decode_max_output(size_t input_length);

} // namespace glue
