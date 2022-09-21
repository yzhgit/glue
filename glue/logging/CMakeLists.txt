#
# Copyright © 2017 zhangyao. All rights reserved.
# SPDX-License-Identifier: MIT
#

add_definitions(-D_AMD64_)

add_executable(glog
    helpers.h
    helpers.cpp
    log_sink.h
    log_sink.cpp
    logger.h
    logger.cpp
    types.h
    test.cpp
)
