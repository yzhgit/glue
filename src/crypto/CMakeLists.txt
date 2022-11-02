#
# Copyright © 2017 zhangyao. All rights reserved.
# SPDX-License-Identifier: MIT
#

add_definitions(-D_AMD64_)

add_executable(aes aes.h aes.cpp aes_test.cpp)

add_executable(md5 md5_test.cpp hex.h hex.cpp sha256.h sha256.cpp sha512.h sha512.cpp)

add_executable(base64 base64.h base64.cpp base64_test.cpp)
