//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/crypto/base64.h"

#include <catch2/catch.hpp>

#include <iostream>

namespace glue {
namespace test {

    std::vector<std::pair<std::string, std::string>> datas = {
        {"fo", "Zm8="},
        {"foobar", "Zm9vYmFy"},
        {"Man is distinguished, not only by his reason, but by this singular passion from other "
         "animals, which is a lust of the mind, that by a perseverance of delight in the continued "
         "and indefatigable generation of knowledge, exceeds the short vehemence of any carnal "
         "pleasure.",
         "TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0aGlzIHNpbmd1bGF"
         "yIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1c3Qgb2YgdGhlIG1pbmQsIHRoYXQgYn"
         "kgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGludWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlb"
         "mVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRoZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBw"
         "bGVhc3VyZS4="}};

    void test(const std::string& binary, const std::string& base64)
    {
        auto b64 = base64_encode((const uint8_t*) binary.data(), binary.length());
        CHECK(b64 == base64);

        auto plain = base64_decode(base64);
        CHECK(plain == binary);
    }

    TEST_CASE("base64_encode")
    {
        for (auto& data : datas) { test(data.first, data.second); }
    }

} // namespace test
} // namespace glue
