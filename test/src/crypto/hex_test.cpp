//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/crypto/hex.h"

#include <catch2/catch.hpp>
#include <iostream>

#include "glue/base/exception.h"

namespace glue {
namespace test {

std::vector<std::string> valid_hex_vec = {
    "01",
    "012345",
    "0123456789",
    "0123456789abcd",
    "0123456789abcdef01",
    "0123456789abcdef012345",
    "0123456789abcdef0123456789",
    "0123456789abcdef0123456789abcdef",
};

void test_valid_hex(const std::string& hex) {
    std::string bin = hex_decode(hex);
    std::string hex2 = hex_encode(bin);
    std::string bin2 = hex_decode(hex2);
    CHECK(bin.length() == bin2.length());
    CHECK(0 == memcmp(bin.c_str(), bin2.c_str(), bin.length()));
    CHECK(hex.length() == hex2.length());
    CHECK(0 == memcmp(hex.c_str(), hex2.c_str(), hex.length()));
}

TEST_CASE("Hex, Valid_Hex") {
    for (auto& hex : valid_hex_vec) {
        test_valid_hex(hex);
    }
}

std::vector<std::string> invalid_hex_vec = {
    "0",
    "012",
};

void test_invalid_hex(const std::string& hex) {
    try {
        std::string bin = hex_decode(hex);
        std::string hex2 = hex_encode(bin);
        std::string bin2 = hex_decode(hex2);
        CHECK(bin.length() == bin2.length());
        CHECK(0 == memcmp(bin.c_str(), bin2.c_str(), bin.length()));
        CHECK(hex.length() == hex2.length());
        CHECK(0 == memcmp(hex.c_str(), hex2.c_str(), hex.length()));
    } catch (const InvalidArgumentException& e) {
        std::cout << "exception: " << e.what() << std::endl;
    }
}

TEST_CASE("Hex, Invalid_Hex") {
    for (auto& hex : invalid_hex_vec) {
        test_invalid_hex(hex);
    }
}

std::vector<std::vector<std::string>> special_hex_vec = {
    {"1X", "10"},
    {"1Y24", "1024"},
    {"123456M8", "12345608"},
};

void test_special_hex(const std::string& left, const std::string& right) {
    try {
        std::string bin = hex_decode(left);
        std::string hex = hex_encode(bin);
        CHECK(hex.length() == right.length());
        CHECK(0 == memcmp(hex.c_str(), right.c_str(), hex.length()));
    } catch (const InvalidArgumentException& e) {
        std::cout << "exception: " << e.what() << std::endl;
    }
}

TEST_CASE("Hex, Test_Special_Hex") {
    for (auto& pair : special_hex_vec) {
        const std::string left = pair[0];
        const std::string right = pair[1];
        test_special_hex(left, right);
    }
}

}  // namespace test
}  // namespace glue
