//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/logging/log_sink.h"
#include <logging/types.h>

namespace glue {
namespace logging {

ConsoleSink::ConsoleSink()
    : isatty_(!!_isatty(_fileno(stdout))), outputStream_(std::cout) {
    // #ifdef _WIN32
    //     if (isatty_) {
    //         outputHandle_ = GetStdHandle(STD_OUTPUT_HANDLE);
    //     }
    // #endif
}

ConsoleSink::~ConsoleSink() {
    // #ifdef _WIN32
    //     if (!isatty_) {
    outputStream_.flush();
    //     }
    // #endif
}

void ConsoleSink::log(const std::string &msg) {
    // #ifdef _WIN32
    //     if (isatty_) {
    //         WriteConsoleW(outputHandle_, msg.c_str(),
    //                       static_cast<DWORD>(msg.size()), NULL, NULL);
    //     } else {
    //         std::clog << msg.c_str();
    //     }
    // #else
    outputStream_ << msg;
    // #endif
}

FileSink::FileSink(const std::string &filename) {
    outputStream_.open(filename, std::fstream::out | std::fstream::trunc);
}

FileSink::~FileSink() {
    outputStream_.flush();
    outputStream_.close();
}

void FileSink::log(const std::string &msg) { outputStream_ << msg; }

} // namespace logging
} // namespace glue
