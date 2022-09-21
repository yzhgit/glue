//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "ml/utils.h"

#if !defined(BARE_METAL) && (defined(__arm__) || defined(__aarch64__))

    #include <asm/hwcap.h>
    #include <sys/auxv.h>

#endif

namespace glue {
namespace ml {

// Detect the presence of Neon on Linux
bool NeonDetected() {
#if !defined(BARE_METAL) && (defined(__arm__) || defined(__aarch64__))
    auto hwcaps = getauxval(AT_HWCAP);
#endif

#if !defined(BARE_METAL) && defined(__aarch64__)

    if (hwcaps & HWCAP_ASIMD) {
        // On an arm64 device with Neon.
        return true;
    } else {
        // On an arm64 device without Neon.
        return false;
    }

#endif
#if !defined(BARE_METAL) && defined(__arm__)

    if (hwcaps & HWCAP_NEON) {
        // On an armhf device with Neon.
        return true;
    } else {
        // On an armhf device without Neon.
        return false;
    }

#endif

    // This method of Neon detection is only supported on Linux so in order to
    // prevent a false negative we will return true in cases where detection did
    // not run.
    return true;
}

} // namespace ml
} // namespace glue
