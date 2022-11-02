//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/base/File.h"

namespace glue
{

std::list<std::string> File::readLines(const fs::path& path)
{
    std::list<std::string> lines;
    std::ifstream ifs(path.c_str());
    for (std::string line; std::getline(ifs, line);) { lines.push_back(line); }
    return lines;
}

} // namespace glue
