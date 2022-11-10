//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/common.hpp"

//==============================================================================
/** Current GLUE version number.

    See also glue::getVersion() for a string version.
*/
#define GLUE_MAJOR_VERSION 0
#define GLUE_MINOR_VERSION 1
#define GLUE_PATCH_VERSION 0

/** Current GLUE version number.

    Bits 16 to 32 = major version.
    Bits 8 to 16 = minor version.
    Bits 0 to 8 = point release.

    See also glue::getVersion() for a string version.
*/
#define GLUE_VERSION ((GLUE_MAJOR_VERSION << 16) + (GLUE_MINOR_VERSION << 8) + GLUE_PATCH_VERSION)

namespace glue {
/** Returns the current version of GLUE,
   See also the GLUE_VERSION, GLUE_MAJOR_VERSION and GLUE_MINOR_VERSION macros.
*/
GLUE_API const char* getVersion();

} // namespace glue
