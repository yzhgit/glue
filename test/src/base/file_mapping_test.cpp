// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT

#include "glue/base/file_mapping.h"

#include <catch2/catch.hpp>

namespace glue {
namespace test {

TEST_CASE("FileMapping::FileMapping") {
    // TODO:
}

TEST_CASE("FileMapping::data_and_size") {
    FileMapping fm(__FILE__, FileMapping::READ);
    REQUIRE(fm.getSize() > 0);
    REQUIRE(fm.getData() != nullptr);
}

TEST_CASE("FileMapping::offset_and_length") {
    const uint64_t offset = 22;
    const uint64_t length = 14;
    FileMapping fm(__FILE__, FileMapping::READ, offset, length);
    REQUIRE(fm.getSize() == 14);
    REQUIRE(fm.getData() != nullptr);

    auto data = fm.getData();
    REQUIRE('h' == data[0]);
    REQUIRE('a' == data[1]);
    REQUIRE('n' == data[2]);
}

}  // namespace test
}  // namespace glue
