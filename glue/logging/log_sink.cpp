/*
 * Copyright 2015 zhangyao<mosee.gd@163.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "logging/log_sink.h"
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
