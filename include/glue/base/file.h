//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include <list>

#include "ghc/fs_std.hpp"
#include "glue/base/common.h"

namespace glue {

//==============================================================================
/**
    Represents a local file or directory.

    This class encapsulates the absolute pathname of a file or directory, and
    has methods for finding out about the file and changing its properties.

    To read or write to the file, there are methods for returning an input or
    output stream.

    @see FileInputStream, FileOutputStream

    @tags{Core}
*/
class GLUE_API File final {
   public:
    static std::list<std::string> readLines(const fs::path& path);
};

}  // namespace glue
