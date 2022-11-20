//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/crypto/salsa20.h"

#include <catch2/catch.hpp>

namespace glue {
namespace test {

    TEST_CASE("Salsa20 Test")
    {
        uint8_t key1[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                          0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
        uint8_t out1[] = {0x2D, 0xD5, 0xC3, 0xF7, 0xBA, 0x2B, 0x20, 0xF7, 0x68, 0x02,
                          0x41, 0x0C, 0x68, 0x86, 0x88, 0x89, 0x5A, 0xD8, 0xC1, 0xBD,
                          0x4E, 0xA6, 0xC9, 0xB1, 0x40, 0xFB, 0x9B, 0x90, 0xE2, 0x10,
                          0x49, 0xBF, 0x58, 0x3F, 0x52, 0x79, 0x70, 0xEB, 0xC1};
        uint8_t out[39];

        Salsa20 salsa20;

        salsa20.clear();
        salsa20.set_key(key1, sizeof(key1));
        salsa20.cipher(NULL, out, 0);

        CHECK(!memcmp(out, out1, 39));
    }

} // namespace test
} // namespace glue
