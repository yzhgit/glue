//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/base/Version.h"

GLUE_START_NAMESPACE

std::string getVersion()
{
    // Some basic tests, to keep an eye on things and make sure these types work ok
    // on all platforms. Let me know if any of these assertions fail on your system!
    static_assert(sizeof(pointer_sized_int) == sizeof(void*),
                  "Basic sanity test failed: please report!");
    static_assert(sizeof(int8) == 1, "Basic sanity test failed: please report!");
    static_assert(sizeof(uint8) == 1, "Basic sanity test failed: please report!");
    static_assert(sizeof(int16) == 2, "Basic sanity test failed: please report!");
    static_assert(sizeof(uint16) == 2, "Basic sanity test failed: please report!");
    static_assert(sizeof(int32) == 4, "Basic sanity test failed: please report!");
    static_assert(sizeof(uint32) == 4, "Basic sanity test failed: please report!");
    static_assert(sizeof(int64) == 8, "Basic sanity test failed: please report!");
    static_assert(sizeof(uint64) == 8, "Basic sanity test failed: please report!");

    return "GLUE v" GLUE_STRINGIFY(GLUE_MAJOR_VERSION) "." GLUE_STRINGIFY(
        GLUE_MINOR_VERSION) "." GLUE_STRINGIFY(GLUE_PATCH_VERSION);
}

GLUE_END_NAMESPACE
