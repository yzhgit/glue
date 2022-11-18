//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/crypto/base64.h"

#include <memory.h>
#include <stdio.h>
#include <string.h>

using namespace glue::crypto;

int base64_test()
{
    uint8_t text[3][1024] = {{"fo"},
                          {"foobar"},
                          {"Man is distinguished, not only by his reason, but by this singular "
                           "passion from other animals, which is a lust of the mind, that by a "
                           "perseverance of delight in the continued and indefatigable "
                           "generation of knowledge, exceeds the short vehemence of any carnal "
                           "pleasure."}};
    uint8_t code[3][1024] = {{"Zm8="},
                          {"Zm9vYmFy"},
                          {"TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBie"
                           "SB0aGlz\nIHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBp"
                           "cyBhIGx1c3Qgb2Yg\ndGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGV"
                           "saWdodCBpbiB0aGUgY29udGlu\ndWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb2"
                           "4gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRo\nZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55I"
                           "GNhcm5hbCBwbGVhc3VyZS4="}};
    uint8_t buf[1024];
    size_t buf_len;
    int pass = 1;
    int idx;

    for (idx = 0; idx < 3; idx++)
    {
        size_t text_len = strlen((const char*) text[idx]);
        size_t code_len = strlen((const char*) code[idx]);

        buf_len = base64_encode(text[idx], buf, text_len, 1);
        pass = pass &&
               ((buf_len == code_len) && (buf_len == base64_encode(text[idx], NULL, text_len, 1)));
        pass = pass && !memcmp(code[idx], buf, buf_len);

        buf_len = base64_decode(code[idx], buf, code_len);
        pass = pass &&
               ((buf_len == text_len) && (buf_len == base64_decode(code[idx], NULL, code_len)));
        pass = pass && !memcmp(text[idx], buf, buf_len);
    }

    return (pass);
}

int main()
{
    printf("Base64 tests: %s\n", base64_test() ? "PASSED" : "FAILED");

    return 0;
}
