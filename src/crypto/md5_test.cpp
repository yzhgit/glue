//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/crypto/hex.h"
#include "glue/crypto/sha512.h"

#include <iostream>
#include <stdio.h>
#include <string.h>

using namespace glue::crypto;

int main() {
    std::string plaintext =
        "Developers are always trying to shorten the distance between thinking "
        "of a solution and coding. How much does the integration of hardware "
        "and software in your everyday tooling help?";

    uint8_t md5_output[64];

    SHA_512 md5;
    md5.update((const uint8_t *)plaintext.data(), plaintext.size());
    md5.final(md5_output);

    std::string md5_hex = hex_encode(md5_output, 64);

    std::cout << md5_hex.c_str() << std::endl;

    return (0);
}
