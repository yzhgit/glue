#
# Copyright © 2017 zhangyao. All rights reserved.
# SPDX-License-Identifier: MIT
#

add_definitions(-D_AMD64_)

add_executable(glog
    init.h
    util.h
    sink.h
    console_sink.h
    file_sink.h
    formatter.h
    log.h
    logger.h
    record.h
    severity.h
    test.cpp
)
