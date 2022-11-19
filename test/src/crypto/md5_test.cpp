//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/crypto/md5.h"

#include <catch2/catch.hpp>

namespace glue {
namespace test {

    TEST_CASE("MD5 Test")
    {
        char text1[] = {""};
        char text2[] = {"abc"};
        char text3_1[] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcde"};
        char text3_2[] = {"fghijklmnopqrstuvwxyz0123456789"};
        uint8_t hash1[MD5::DigestSize] = {0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04,
                                          0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e};
        uint8_t hash2[MD5::DigestSize] = {0x90, 0x01, 0x50, 0x98, 0x3c, 0xd2, 0x4f, 0xb0,
                                          0xd6, 0x96, 0x3f, 0x7d, 0x28, 0xe1, 0x7f, 0x72};
        uint8_t hash3[MD5::DigestSize] = {0xd1, 0x74, 0xab, 0x98, 0xd2, 0x77, 0xd9, 0xf5,
                                          0xa5, 0x61, 0x1c, 0x2c, 0x9f, 0x41, 0x9d, 0x9f};
        uint8_t buf[MD5::DigestSize];
        MD5 md5;

        md5.update((uint8_t*) text1, strlen(text1));
        md5.final(buf);
        CHECK(!memcmp(hash1, buf, MD5::DigestSize));

        // Note the MD5 object can be reused.
        md5.clear();
        md5.update((uint8_t*) text2, strlen(text2));
        md5.final(buf);
        CHECK(!memcmp(hash2, buf, MD5::DigestSize));

        // Note the data is being added in two chunks.
        md5.clear();
        md5.update((uint8_t*) text3_1, strlen(text3_1));
        md5.update((uint8_t*) text3_2, strlen(text3_2));
        md5.final(buf);
        CHECK(!memcmp(hash3, buf, MD5::DigestSize));
    }

} // namespace test
} // namespace glue
