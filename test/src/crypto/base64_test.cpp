//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/crypto/base64.h"

#include <catch2/catch.hpp>

namespace glue {
namespace test {

    TEST_CASE("base64_encode")
    {
        auto str1 = base64_encode((const uint8_t*) "fo", 2);
        CHECK(str1 == "Zm8=");

        auto str2 = base64_encode((const uint8_t*) "foobar", 6);
        CHECK(str2 == "Zm9vYmFy");

        auto str3 = base64_encode(
            (const uint8_t*) "Man is distinguished, not only by his reason, but by this singular "
                             "passion from other animals, which is a lust of the mind, that by a "
                             "perseverance of delight in the continued and indefatigable "
                             "generation "
                             "of knowledge, exceeds the short vehemence of any carnal pleasure.",
            269);
        CHECK(str3 == "TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0aGlz"
                      "IHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1c3Qgb2Yg"
                      "dGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGlu"
                      "dWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRo"
                      "ZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4=");
    }

} // namespace test
} // namespace glue
