#
# Copyright © 2017 zhangyao. All rights reserved.
# SPDX-License-Identifier: MIT
#

add_definitions(-D_AMD64_)

add_executable(aes aes.h aes.cpp aes_test.cpp)
add_executable(md5 md5.h md5.cpp md5_test.cpp)
