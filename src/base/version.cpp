//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/base/version.h"

namespace glue {

const char* getVersion()
{
    return "GLUE v" GLUE_STRINGIFY(GLUE_MAJOR_VERSION) "." GLUE_STRINGIFY(
        GLUE_MINOR_VERSION) "." GLUE_STRINGIFY(GLUE_PATCH_VERSION);
}

} // namespace glue
