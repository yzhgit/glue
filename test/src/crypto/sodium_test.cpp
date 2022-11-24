
//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/crypto/sodium.h"

#include <catch2/catch.hpp>

namespace glue {
namespace test {

    uint8_t key[crypto_secretbox_KEYBYTES] = {0x2b, 0x27, 0x55, 0x64, 0x73, 0xe9, 0x85, 0xd4,
                                              0x62, 0xcd, 0x51, 0x19, 0x7a, 0x9a, 0x46, 0xc7,
                                              0x60, 0x09, 0x54, 0x9e, 0xac, 0x64, 0x74, 0xf2,
                                              0x06, 0xc4, 0xee, 0x08, 0x44, 0xf6, 0x83, 0x89};
    uint8_t nonce[crypto_secretbox_NONCEBYTES] = {0x79, 0x69, 0x6e, 0xe9, 0x55, 0xb6, 0x2b, 0x73,
                                                  0xcd, 0x62, 0xbd, 0xa8, 0x75, 0xfc, 0x73, 0xd6,
                                                  0x82, 0x19, 0xe0, 0x03, 0x6b, 0x7a, 0x0b, 0x37};

    std::string encrypt(const std::string& m)
    {
        size_t length = m.length() + crypto_secretbox_MACBYTES;
        std::string c(length, 0);
        crypto_secretbox_easy((uint8_t*) c.data(), (uint8_t*) m.data(), m.length(), nonce, key);
        return c;
    }

    std::string decrypt(const std::string& m)
    {
        size_t length = m.length() - crypto_secretbox_MACBYTES;
        std::string c(length, 0);
        crypto_secretbox_open_easy((uint8_t*) c.data(), (uint8_t*) m.data(), m.length(), nonce,
                                   key);
        return c;
    }

    TEST_CASE("sodium")
    {
        std::string plain_text = "hello world!";
        std::string cipher_text = encrypt(plain_text);
        std::string decrypt_text = decrypt(cipher_text);
        CHECK(decrypt_text == plain_text);
    }

} // namespace test
} // namespace glue
